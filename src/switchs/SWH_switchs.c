#include "SWH_switchs.h"

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32469i_discovery.h"

#include "./../led/LED_led.h"
#include "./../ihm/IHM_match.h"

#include "./../audio/audio_system.h"
#include "./../pokraphic/pokraphic.h"
#include "./../pokuicom/COM_pokuicom.h"

#include <stdlib.h>

extern ADC_HandleTypeDef hadc1;



int tirette_state = -1;
int switch1_state = -1;
int switch2_state = -1;
int switch3_state = -1;

int match_started = 0;


int adcValueVolume = 0;

int rgbStateLed = 0;


// ADC interuption
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    int adcValue = HAL_ADC_GetValue(&hadc1);

    // Some kind of cheap low pass filter
    adcValue = (adcValue + 7*adcValueVolume) / 8;

    // Refresh only if the new value is greater than a defined value.
    // This condition is some kind of hysteresis, to not continuously change the setting,
    // and only change the volume when the potentiometer is turned by someone.
    if (abs(adcValueVolume-adcValue) > 32) {
        adcValueVolume = adcValue;
        // Converting 12 bit value to 8 bit
        // From 0-4095 to 0-255
        ASYS_setVolume(adcValueVolume/16);
    }

}

void SWH_init() {
    // Squalala
}

void SWH_tick() {
    int st;

    // Start ADC conversion
    HAL_ADC_Start_IT(&hadc1);


    // Tirette
    st = HAL_GPIO_ReadPin(TIRETTE_GPIO_Port, TIRETTE_Pin);
    if (tirette_state != st) {
        if (tirette_state == 1 && st == 0) {
            // Tirette has been removed: starting match
            LED_setColor(LED_BOTTOM_MIDDLE, 0x00, 0x00, 0xFF);
            ASYS_playFile("brenda/squalala_nous_sommes_partis.wav", 200);
            PCOM_notify_start_of_match();
            IHM_matchTiretteReleased();
        } else {
            if (st == 0) {
                // No tirette when booting
                LED_setColor(LED_BOTTOM_MIDDLE, 0xFF, 0x00, 0x00);
                LED_setBlink(LED_BOTTOM_MIDDLE, 1, 0, 0);
            } else {
                // Tirette has been placed
                ASYS_playFile("sound/ho-je-bouge-plus.wav", 190);
                LED_setColor(LED_BOTTOM_MIDDLE, 0x00, 0xFF, 0x00);
                IHM_matchTirettePlugged();
            }
        }
        tirette_state = st;
    }
    // Switch 1
    st = HAL_GPIO_ReadPin(SWITCH1_GPIO_Port, SWITCH1_Pin);
    if (switch1_state != st) {
        if (st == 0) {
            ASYS_playFile("sound/im-blue-dabadee.wav", 120);
            PCOM_send_team_color(POKTOCOL_TEAM_BLUE);
        } else if (st == 1) {
            ASYS_playFile("sound/eleonore_content_jus_orange.wav", 120);
            PCOM_send_team_color(POKTOCOL_TEAM_YELLOW);
        }
        IHM_matchSetColor(st + 1);
        switch1_state = st;
    }
    // Switch 2
    st = HAL_GPIO_ReadPin(SWITCH2_GPIO_Port, SWITCH2_Pin);
    if (switch2_state != st) {
        if (st == 1) {
            PKC_setRGBMode(1);
            ASYS_repeatFile("fluf.wav", 0, 714000, 190);
        } else if (st == 0) {
            PKC_setRGBMode(0);
            if (switch2_state != -1) ASYS_stop(190);
        }
        switch2_state = st;
    }
    // Switch 3
    st = HAL_GPIO_ReadPin(SWITCH3_GPIO_Port, SWITCH3_Pin);
    if (switch3_state != st) {
        if (st == 0) {
            if (switch3_state != -1) ASYS_playFile("windaube/windows-error.wav", 200);
        } else if (st == 1) {
            if (switch3_state != -1) ASYS_playFile("windaube/windows-error.wav", 200);
        }
        switch3_state = st;
    }

    rgbStateLed = (rgbStateLed + 1) & 0xFF;
    uint32_t color = BSP_LCD_getRainbowColor(rgbStateLed);
    LED_setColor(LED_TOP_MIDDLE, (color>>16)&0xFF, (color>>8)&0xFF, (color>>0)&0xFF);
}