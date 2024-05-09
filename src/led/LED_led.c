#include "LED_led.h"

#include "stm32f4xx_hal.h"

//==============================//
// Configuration
//==============================//

#define TLC59116_I2C_ADDR (0x60<<1)

#define AUTO_INC 0x80

//==============================//
// Global variables
//==============================//

extern I2C_HandleTypeDef hi2c1;

int LED_status = 1;

int reg_buffer_update = 1;
uint8_t reg_buffer[0x18] = {0};

//==============================//
// Private I2C functions
//==============================//

void LED_write(int addr, uint8_t value) {
    HAL_I2C_Mem_Write(&hi2c1, TLC59116_I2C_ADDR, addr | AUTO_INC, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
}
uint8_t LED_read(int addr) {
    uint8_t value;
    HAL_I2C_Mem_Read(&hi2c1, TLC59116_I2C_ADDR, addr | AUTO_INC, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
    return value;
}
void LED_write_dma(int addr, uint8_t* buffer, int length) {
    HAL_I2C_Mem_Write_IT(&hi2c1, TLC59116_I2C_ADDR, addr | AUTO_INC, I2C_MEMADD_SIZE_8BIT, buffer, length);
}


//==============================//
// Public functions
//==============================//


int LED_init() {
    uint8_t reg0;
    LED_write(0x00, 0x00);
    reg0 = LED_read(0x00);
    if (reg0 == 0x80) {
        LED_status = 0;
    } else {
        printf("TLC59116 is not responding. LEDs are not available.\r\n");
        LED_status = 1;
        return LED_status;
    }

    // Enable OSC, disable all call address response
    LED_write(0x00, 0x00);
    LED_write(0x01, (1<<5));

    LED_setBlink(LED_TOP_LEFT, 0, 0, 0);
    LED_setBlink(LED_TOP_MIDDLE, 0, 0, 0);
    LED_setBlink(LED_TOP_RIGHT, 1, 0, 0);
    LED_setBlink(LED_BOTTOM_LEFT, 0, 0, 0);
    LED_setBlink(LED_BOTTOM_MIDDLE, 0, 0, 0);
    LED_setBlink(LED_BOTTOM_RIGHT, 0, 0, 0);

    LED_setBrightness(0x80);
    LED_setBlinkRate(500);

    LED_update();

    return LED_status;
}

void LED_setColor(int led_index, int red, int green, int blue) {
    red = red / 4;
    green = green / 4;
    blue = blue / 4;
    switch (led_index) {
        case LED_TOP_LEFT:
            reg_buffer[0x0F] = blue;
            reg_buffer[0x10] = green;
            reg_buffer[0x11] = red;
            break;
        case LED_TOP_MIDDLE:
            reg_buffer[0x02] = blue;
            reg_buffer[0x03] = green;
            reg_buffer[0x04] = red;
            break;
        case LED_TOP_RIGHT:
            // 1 LED only
            reg_buffer[0x0E] = red;
            break;
        case LED_BOTTOM_LEFT:
            reg_buffer[0x0B] = red;
            reg_buffer[0x0C] = green;
            reg_buffer[0x0D] = blue;
            break;
        case LED_BOTTOM_MIDDLE:
            reg_buffer[0x08] = red;
            reg_buffer[0x09] = green;
            reg_buffer[0x0A] = blue;
            break;
        case LED_BOTTOM_RIGHT:
            reg_buffer[0x05] = red;
            reg_buffer[0x06] = green;
            reg_buffer[0x07] = blue;
            break;
    }
    reg_buffer_update = 1;
}
void LED_setBlink(int led_index, int red_blink, int green_blink, int blue_blink) {
    switch (led_index) {
        case LED_TOP_LEFT:
            reg_buffer[0x17] = (reg_buffer[0x17]&0x03) | 0xA8 | ((red_blink&1)<<6) | ((green_blink&1)<<4) | ((blue_blink&1)<<2);
            break;
        case LED_TOP_MIDDLE:
            reg_buffer[0x14] = (reg_buffer[0x14]&0xC0) | 0x2A | ((red_blink&1)<<4) | ((green_blink&1)<<2) | ((blue_blink&1)<<0);
            break;
        case LED_TOP_RIGHT:
            // 1 LED only
            reg_buffer[0x17] = (reg_buffer[0x17]&0xFC) | 0x02 | ((red_blink&1)<<0);
            break;
        case LED_BOTTOM_LEFT:
            reg_buffer[0x16] = (reg_buffer[0x16]&0x03) | 0xA8 | ((blue_blink&1)<<6) | ((green_blink&1)<<4) | ((red_blink&1)<<2);
            break;
        case LED_BOTTOM_MIDDLE:
            reg_buffer[0x15] = (reg_buffer[0x15]&0x0F) | 0xA0 | ((green_blink&1)<<6) | ((red_blink&1)<<4);
            reg_buffer[0x16] = (reg_buffer[0x16]&0xFC) | 0x02 | ((blue_blink&1)<<0);
            break;
        case LED_BOTTOM_RIGHT:
            reg_buffer[0x14] = (reg_buffer[0x14]&0x3F) | 0x80 | ((red_blink&1)<<6);
            reg_buffer[0x15] = (reg_buffer[0x15]&0xF0) | 0x0A | ((blue_blink&1)<<2) | ((green_blink&1)<<0);
            break;
    }
    reg_buffer_update = 1;
}
void LED_update() {
    if (reg_buffer_update) {
        reg_buffer_update = 0;
        LED_write_dma(0x02, reg_buffer+2, 22);
    }
}

void LED_setBrightness(uint8_t value) {
    reg_buffer[0x12] = value;
    reg_buffer_update = 1;
}
void LED_setBlinkRate(int period_ms) {
    uint8_t value = (24*period_ms/1000) - 1;
    reg_buffer[0x13] = value;
    reg_buffer_update = 1;
}