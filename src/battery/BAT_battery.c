#include "BAT_battery.h"

#include "stm32f4xx_hal.h"

#include "./../led/LED_led.h"

#include "./../audio/audio_system.h"

//==============================//
// Configuration
//==============================//

#define INA220_I2C_ADDR (0x40<<1)

#define SHUNT_RESISTOR 0.002 // ohms
#define CURRENT_LSB (0.001) // amps
#define CALREG_VALUE ((uint16_t)(0.04096/(CURRENT_LSB*SHUNT_RESISTOR)))


#define LIPO_WARNING_VOLTAGE 16000
#define LIPO_CRITICAL_VOLTAGE 15000

#define LIPO_CRITICAL_MUSIC_HYSTERESIS 50

//==============================//
// Global variables
//==============================//

BAT_dataPoint BAT_lastData;
int BAT_emergencyStopDetectionCurrentValue = 500;

extern I2C_HandleTypeDef hi2c1;
int BAT_dma_request;

// Buffer for I2C read operations
uint8_t BAT_i2c_buffer[2];
int BAT_readStep = 0;

int BAT_status = 1;

uint32_t argb_color = 0;

int BAT_criticalFlag = 0;

//==============================//
// Private I2C functions
//==============================//

void BAT_write(int addr, uint16_t value) {
    uint8_t buffer[2] = {value>>8, value&0xFF};
    HAL_I2C_Mem_Write(&hi2c1, INA220_I2C_ADDR, addr, I2C_MEMADD_SIZE_8BIT, buffer, 2, 100);
}
uint16_t BAT_read(int addr) {
    uint8_t buffer[2];
    HAL_I2C_Mem_Read(&hi2c1, INA220_I2C_ADDR, addr, I2C_MEMADD_SIZE_8BIT, buffer, 2, 100);
    return (((uint16_t)buffer[0]) << 8) | ((uint16_t)buffer[1]);
}
void BAT_read_dma(int addr, uint8_t* buffer, int length) {
    if (HAL_I2C_Mem_Read_DMA(&hi2c1, INA220_I2C_ADDR, addr, I2C_MEMADD_SIZE_8BIT, buffer, length) == HAL_OK) {
        BAT_dma_request = 1;
    }
}

//==============================//
// Public functions
//==============================//

int BAT_init() {
    BAT_lastData.voltage = 16200;
    BAT_lastData.current = 0;

    BAT_readStep = 0;

    uint16_t reg0 = BAT_read(0x00);

    if (reg0 == 0x399F || reg0 == 0x219F) {
        BAT_status = 0;
    } else {
        printf("INA220 is not responding. Voltage and current measurements are not available.");
        BAT_status = 1;
        return BAT_status;
    }

    // Set PGA to x1, other parameters have default values
    BAT_write(0x00, 0x219F);
    // Set callibration value for current measurement
    BAT_write(0x05, CALREG_VALUE);

    return BAT_status;
}

void BAT_update() {
    if (BAT_status == 0) {
        // Read voltage
        if (BAT_readStep == 2) {
            BAT_read_dma(0x02, BAT_i2c_buffer, 2);
            BAT_readStep = 1;
        } else {
            BAT_read_dma(0x04, BAT_i2c_buffer, 2);
            BAT_readStep = 2;
        }
        uint32_t c = BAT_getColorIndicator();
        if (c != argb_color) {
            argb_color = c;
            LED_setColor(LED_BOTTOM_RIGHT, (argb_color>>16)&0xFF, (argb_color>>8)&0xFF, (argb_color>>0)&0xFF);
        }

        if (BAT_criticalFlag) {
            if (BAT_lastData.voltage > LIPO_CRITICAL_VOLTAGE + LIPO_CRITICAL_MUSIC_HYSTERESIS) {
                BAT_criticalFlag = 0;
                ASYS_stop(250);
            }
        } else {
            if (BAT_lastData.voltage < LIPO_CRITICAL_VOLTAGE - LIPO_CRITICAL_MUSIC_HYSTERESIS) {
                BAT_criticalFlag = 1;
                ASYS_repeatFile("sound/lalipo.wav", 0, 833500, 250);
            }
        }
    }
}
void BAT_dma_callback() {
    BAT_dma_request = 0;
    if (BAT_readStep == 1) {
        //if (BAT_i2c_buffer[0] && BAT_i2c_buffer[1]) {
            BAT_lastData.voltage = ((int)(BAT_i2c_buffer[0])<<5) | ((int)(BAT_i2c_buffer[1])>>3);
            BAT_lastData.voltage = BAT_lastData.voltage * 4;
        //}
        // Read current
        // BAT_read_dma(0x04, BAT_i2c_buffer, 2);
        // BAT_readStep++;
    } else if (BAT_readStep == 2) {
        //printf("2\r\n");
        //if (BAT_i2c_buffer[0] && BAT_i2c_buffer[1]) {
            BAT_lastData.current = ((int)(BAT_i2c_buffer[0])<<8) | ((int)(BAT_i2c_buffer[1]));
        //}
        // BAT_readStep = 0;
    }
}

int BAT_getVoltage() {
    return BAT_lastData.voltage;
}
int BAT_getCurrent() {
    return BAT_lastData.current;
}
int BAT_isEmergencyStopReleased() {
    return BAT_lastData.current > BAT_emergencyStopDetectionCurrentValue;
}
void BAT_setEmergencyStopDetectionCurrentValue(int c) {
    BAT_emergencyStopDetectionCurrentValue = c;
}

int BAT_lipoAtWarningVoltage() {
    return BAT_lastData.voltage < LIPO_WARNING_VOLTAGE;
}
int BAT_lipoAtCriticalVoltage() {
    return BAT_lastData.voltage < LIPO_CRITICAL_VOLTAGE;
}

uint32_t BAT_getColorIndicator() {
    if (BAT_lipoAtCriticalVoltage()) {
        return 0xFFFF0000;
    } else if (BAT_lipoAtWarningVoltage()) {
        return 0xFFFF8000;
    } else {
        return 0xFF00FF00;
    }
}