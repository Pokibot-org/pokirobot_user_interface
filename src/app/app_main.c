#include "app_main.h"

#include "main.h"

#include <string.h>
#include <stdio.h>

#include "stm32f4xx_hal.h"

#include "stm32469i_discovery.h"

#include "fatfs.h"

#include "./../pokraphic/pokraphic.h"
#include "./../pokraphic/PKC_poki.h"
#include "./../pokraphic/PKC_button.h"
#include "./../pokraphic/PKC_slider.h"
#include "./../pokraphic/PKC_combox.h"
#include "./../pokraphic/PKC_scrollList.h"
#include "./../pokraphic/PKC_segments.h"
#include "./../pokraphic/PKC_numPad.h"

#include "./../touchscreen/FT6336G.h"
#include "./../audio/audio_system.h"
// #include "./../audio/microphone.h"

#include "./../VL53L5CX/vlc53l5cx_app.h"

#include "./../ihm/IHM_themes.h"
#include "./../ihm/IHM_bomb.h"
#include "./../ihm/IHM_match.h"
#include "./../ihm/IHM_notification.h"
#include "./../ihm/IHM_soundboard.h"

//#include "./../led_strip/ARGB.h"

#include "./../battery/BAT_battery.h"
#include "./../rtc/RTC_clock.h"
#include "./../led/LED_led.h"
#include "./../switchs/SWH_switchs.h"


#include "../pokuicom/COM_pokuicom.h"

extern TIM_HandleTypeDef htim3;


const uint32_t poki_160x160_argb8888 = LAYER0_ADDRESS + LCD_BUFFER_SIZE*2;


extern VL53L5CX_ResultsData TOF_results;
extern int TOF_diffs[4];

extern RTC_time rtc_time;

extern PKC_themes PKC_theme;
extern uint32_t PKC_soundFlags;

PKC_item poki;
PKC_pokiP pokiParams0;

char buttonOptionsText[] = "Options";
PKC_buttonP buttonOptionsParams;
PKC_item buttonOptions;
char buttonBombText[] = "Mode bombe";
PKC_buttonP buttonBombParams;
PKC_item buttonBomb;
char buttonMatchText[] = "Mode match";
PKC_buttonP buttonMatchParams;
PKC_item buttonMatch;
char buttonSoundboardText[] = "Soundboard";
PKC_buttonP buttonSoundboardParams;
PKC_item buttonSoundboard;


PKC_item numPad;
PKC_numPadP numPadParams;

int mainIHMIsOpen = 1;
void mainIHMOpen() {
    PKC_addItem(&buttonOptions);
    PKC_addItem(&buttonBomb);
    PKC_addItem(&buttonMatch);
    PKC_addItem(&buttonSoundboard);
    mainIHMIsOpen = 1;
}
void mainIHMClose() {
    PKC_removeItem(&buttonOptions);
    PKC_removeItem(&buttonBomb);
    PKC_removeItem(&buttonMatch);
    PKC_removeItem(&buttonSoundboard);
    mainIHMIsOpen = 0;
}

void paramCloseCallback() {
    IHM_themesClose();
    mainIHMOpen();
}
void bombCloseCallback() {
    IHM_bombClose();
    mainIHMOpen();
}
void matchCloseCallback() {
    IHM_matchClose();
    mainIHMOpen();
}
void soundboardCloseCallback() {
    IHM_soundboardClose();
    mainIHMOpen();
}

void buttonOptionsCallback(int event) {
    if (event&PKC_BUTTON_RELEASE) {
        mainIHMClose();
        IHM_themesOpen();
    }
}
void numPadBombCallback(int mode, float value) {
    if (mode) {
        mainIHMClose();
        IHM_bombOpen((int)value);
    }
}
void buttonBombCallback(int event) {
    if (event&PKC_BUTTON_RELEASE) {
        PKC_numPadOpen(&numPad, 0, "Bomb timer", &numPadBombCallback);
    }
}
void buttonMatchCallback(int event) {
    if (event&PKC_BUTTON_RELEASE) {
        mainIHMClose();
        IHM_matchOpen();
    }
}
void APP_forceMatchIHM() {
    if (mainIHMIsOpen) {
        buttonMatchCallback(PKC_BUTTON_RELEASE);
    }
}
void buttonSoundboardCallback(int event) {
    if (event&PKC_BUTTON_RELEASE) {
        mainIHMClose();
        IHM_soundboardOpen();
    }
}


volatile int datasentflag = 0;

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    printf("kqdjsfhksdjhfjk\r\n");
    HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_4);
    datasentflag = 1;
}
void HAL_TIM_ErrorCallback (TIM_HandleTypeDef * htim) {
    printf("kqdjsfhksdjhfjk\r\n");
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    printf("kqdjsfhksdjhfjk\r\n");
    HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_4);
    datasentflag = 1;
}

uint16_t pwmData[24];
void WS2812_Send (uint32_t color) {
    for (int i = 23; i >= 0; i--) {
        if (color&(1<<i)) {
            pwmData[i] = 66;
        } else {
            pwmData[i] = 33;
        }
    }
    HAL_StatusTypeDef sdjkfhd = HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_4, (uint32_t *)pwmData, 24);
    printf("qsdjklfsdljkfh %d\r\n", sdjkfhd);
    while (!datasentflag);
    datasentflag = 0;
}





int app_main(void) {

    printf("App start\r\n");

    HAL_Delay(100);


    // HAL_TIM_Base_Start(&htim3);

    // HAL_Delay(100);
    // WS2812_Send(0x00FF00FF);
    // HAL_Delay(100);

    // ARGB_Init();
    // ARGB_Clear(); // Clear stirp
    // while (ARGB_Show() != ARGB_OK); // Update - Option 1
    // ARGB_SetBrightness(100);  // Set global brightness to 40%
    // ARGB_SetRGB(2, 0, 255, 0); // Set LED with 255 Green
    // while (!ARGB_Show());  // Update - Option 2
    // printf("Loop\r\n");
    // while(1);

    // HAL_TIM_Base_Start(&htim4);
    // HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);




    // Battery system init
    BAT_init();

    // Init real time clock
    RTC_init();
    // Uncomment these lines to set the time and date at startup:
    // RTC_time newTime;
    // newTime.seconds  = 0;
    // newTime.minutes  = 03;
    // newTime.hours    = 19;
    // newTime.days     = 26;
    // newTime.weekdays = 4;
    // newTime.months   = 4;
    // newTime.years    = 24;
    // RTC_set(newTime);

    // Init LEDs
    LED_init();

    // Switch off 4 leds
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1);
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, 1);

    // Switchs, analog inputs
    SWH_init();

    // Touch screen controller callback
    FT6336G_setUpdateCallback(PKC_tsEvent);

    // Init pokuicom
    PCOM_init();

    // Init VLC53L5CX
    // if (TOF_init() != 0) {
    //     printf("VLC53L5CX not working\r\n");
    // } else {
    //     printf("VLC53L5CX is OK\r\n");
    // }

    ASYS_init();

    // Mount SD card
    FATFS FatFs;
    int r = f_mount(&FatFs, "", 1);
    if (r == FR_OK) {
        printf("SD card OK\r\n");
    } else {
        printf("SD card not found\r\n");
    }


    // Init POKRAPHIC system
    PKC_init();

    PKC_pokiInit(
        &poki, 0, 0, 160, 160, 15,
        &pokiParams0, ((uint32_t*)poki_160x160_argb8888 + 480*480)
    );
    PKC_pokiSetPic(&poki, "pics/poki_480.bin");
    PKC_addItem(&poki);

    PKC_buttonInit(
        &buttonOptions, 10, 480-60-10, 200, 60, 10,
        &buttonOptionsParams, (char*)buttonOptionsText, &buttonOptionsCallback
    );
    PKC_buttonInit(
        &buttonBomb, 10, 480-60-10-70, 200, 60, 10,
        &buttonBombParams, (char*)buttonBombText, &buttonBombCallback
    );
    PKC_buttonInit(
        &buttonMatch, 10, 480-60-10-70-70, 200, 60, 10,
        &buttonMatchParams, (char*)buttonMatchText, &buttonMatchCallback
    );
    PKC_buttonInit(
        &buttonSoundboard, 10, 480-60-10-70-70-70, 200, 60, 10,
        &buttonSoundboardParams, (char*)buttonSoundboardText, &buttonSoundboardCallback
    );


    // Global numeric pad
    PKC_numPadInit(
        &numPad, 12,
        &numPadParams
    );


    // Init all sub IHM
    IHM_bombInit(&bombCloseCallback);
    IHM_themesInit(&paramCloseCallback);
    // IMH_notificationInit();
    IHM_matchInit(&matchCloseCallback);
    IHM_soundboardInit(&soundboardCloseCallback);
    // Open the main IHM
    mainIHMOpen();

    PKC_addRefreshZoneAll();



    RTC_update();
    HAL_Delay(100);

    printf("Time: %02d:%02d:%02d\r\n", rtc_time.hours, rtc_time.minutes, rtc_time.seconds);
    printf("Date: %02d/%02d/%02d\r\n", rtc_time.days, rtc_time.months, rtc_time.years);
    printf("Weekday: %02d\r\n", rtc_time.weekdays);

    // Boot sound
    if (rtc_time.weekdays == 2) { // If wednesday
        ASYS_playFile("WEDN.WAV", 200);
    } else {
        ASYS_playFile("windaube/windows-startup.wav", 200);
    }

    // Main loop, running UI
    int loopCounter = 0;
    printf("Entering main loop\r\n");
    while (1) {
        if (loopCounter%4 == 0) {
            LED_update();
        }
        if (loopCounter%10 == 1) {
            RTC_update();
        }
        if (loopCounter%20 == 2) {
            BAT_update();
        }

        if (loopCounter%10 == 3) {
            PCOM_tick();
        }

        SWH_tick();

        IHM_bombTick();
        IHM_matchTick();

        PKC_tick();
        PKC_refresh();

        loopCounter++;
    }
}


// I2C1 DMA callback dispatch
extern int RTC_dma_request;
extern int FT6336G_dma_request;
extern int BAT_dma_request;
// extern int LED_dma_request;
// This called after I2C DMA read operation
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef * hi2c) {
    if (RTC_dma_request) {
        RTC_dma_callback();
    }
    if (FT6336G_dma_request) {
        FT6336G_dma_callback();
    }
    if (BAT_dma_request) {
        BAT_dma_callback();
    }
}