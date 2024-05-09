#include "FT6336G.h"

#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdlib.h>

//==============================//
// Configuration
//==============================//

#define WIDTH 800
#define HEIGHT 480

#define SWAP_XY
// Axis flip is done after X/Y swap (if enabled)
// #define FLIP_X
#define FLIP_Y

#define FT6336_I2C_ADDR 0x70

//==============================//
// Some register values
//==============================//

#define FT6336G_PRESS_DOWN 0x00
#define FT6336G_LIFT_UP 0x01
#define FT6336G_CONTACT 0x02
#define FT6336G_NO_EVENT 0x03

//==============================//
//
//==============================//

int FT6336G_dma_request;

extern I2C_HandleTypeDef hi2c1;

FT6336G_point p0, p1;
// Buffer for I2C read operations with DMA
uint8_t buffer[15];

void (*updateCallback)(void) = 0;

int FT6336G_event;

void FT6336G_setUpdateCallback(void(*callback)(void)) {
    updateCallback = callback;
}

void FT6336G_updatePoint(FT6336G_point* p, uint8_t* localBuffer) {
    p->prevx = p->x;
    p->prevy = p->y;

#ifndef SWAP_XY
    p->x = ((localBuffer[0x03] & 0x0F) << 8) | localBuffer[0x04];
    p->y = ((localBuffer[0x05] & 0x0F) << 8) | localBuffer[0x06];
#else
    p->x = ((localBuffer[0x05] & 0x0F) << 8) | localBuffer[0x06];
    p->y = ((localBuffer[0x03] & 0x0F) << 8) | localBuffer[0x04];
#endif
#ifdef FLIP_X
    p->x = WIDTH-1-p->x;
#endif
#ifdef FLIP_Y
    p->y = HEIGHT-1-p->y;
#endif

    FT6336G_event = localBuffer[0x03]>>6;
    switch (FT6336G_event) {
        case FT6336G_PRESS_DOWN:
            p->event = TS_PRESS_DOWN;
            break;
        case FT6336G_LIFT_UP:
            p->event = TS_LIFT_UP | TS_REAL_LIFT_UP;
            break;
        case FT6336G_CONTACT:
            p->event = TS_CONTACT;
            break;
        default:
            p->event = 0;
            break;
    }

    if (p->event & TS_PRESS_DOWN) {
        p->dx = 0;
        p->dy = 0;
        p->dxSum = 0;
        p->dySum = 0;
        p->timer = 0;
        p->longPress = 0;
        p->currentItemIndex = -1;
    } else if (p->event & TS_CONTACT) {
        p->dx = p->x - p->prevx;
        p->dy = p->y - p->prevy;
        p->dxSum = p->dxSum + p->dx;
        p->dySum = p->dySum + p->dy;
        p->timer = p->timer + 1;
        if (p->timer == 50 && abs(p->dxSum) + abs(p->dySum) < 20) {
            p->longPress = 1;
        }
    }
    p->tapCondition = (abs(p->dxSum) + abs(p->dySum) < 5);
}


// This called after DMA read operation
void FT6336G_dma_callback() {
    FT6336G_dma_request = 0;

    FT6336G_updatePoint(&p0, buffer);
    FT6336G_updatePoint(&p1, buffer+6);

    if (updateCallback) {
        (*updateCallback)();
    }
}

void FT6336G_readData() {
    // Read data using DMA
    HAL_I2C_Mem_Read_DMA(&hi2c1, FT6336_I2C_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, buffer, 15);
    FT6336G_dma_request = 1;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_5) {
        // If interrupt is detected, refresh point data by reading I2C registers
        FT6336G_readData();
    }
}

FT6336G_point FT6336G_getP0() {
    return p0;
}
FT6336G_point FT6336G_getP1() {
    return p1;
}

int FT6336G_getNbrPoints() {
    if (p0.event == TS_NO_EVENT_VALUE) {
        return 0;
    } else if (p1.event == TS_NO_EVENT_VALUE) {
        return 1;
    } else {
        return 2;
    }
}
