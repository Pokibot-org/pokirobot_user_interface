#include "PKC_slider.h"

#include "./../audio/audio_system.h"

#define EDGE_WIDTH 5

void PKC_sliderInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_sliderP* sliderParams, void (*callback)(int value)
) {
    PKC_itemInit(item, x, y, width, height, level, &PKC_sliderDraw, &PKC_sliderTsEvent, &PKC_sliderTick);
    item->params = sliderParams;
    if (width > height) {
        sliderParams->value = 0;
    } else {
        sliderParams->value = height - EDGE_WIDTH*2 - 1;
    }
    sliderParams->callback = callback;
    sliderParams->event = 0;
}

void PKC_sliderDraw(PKC_item* item) {
    PKC_sliderP* sliderParams = (PKC_sliderP*)(item->params);
    BSP_LCD_SetTextColor(item->theme->color0);
    BSP_LCD_FillRect(item->x, item->y, item->width, item->height);

    if (item->width > item->height) {
        if (item->width - sliderParams->value > 0) {
            BSP_LCD_SetTextColor(item->theme->color2);
            BSP_LCD_FillRect(item->x + EDGE_WIDTH + sliderParams->value, item->y + EDGE_WIDTH, item->width - sliderParams->value - 2*EDGE_WIDTH, item->height - 2*EDGE_WIDTH);
        }
        if (sliderParams->value > 0) {
            BSP_LCD_SetTextColor(item->theme->color1);
            BSP_LCD_FillRect(item->x + EDGE_WIDTH, item->y + EDGE_WIDTH, sliderParams->value, item->height - 2*EDGE_WIDTH);
        }
    } else {
        if (item->height - sliderParams->value > 0) {
            BSP_LCD_SetTextColor(item->theme->color1);
            BSP_LCD_FillRect(item->x + EDGE_WIDTH, item->y + EDGE_WIDTH + sliderParams->value, item->width - 2*EDGE_WIDTH, item->height - sliderParams->value - 2*EDGE_WIDTH);
        }
        if (sliderParams->value > 0) {
            BSP_LCD_SetTextColor(item->theme->color2);
            BSP_LCD_FillRect(item->x + EDGE_WIDTH, item->y + EDGE_WIDTH, item->width - 2*EDGE_WIDTH, sliderParams->value);
        }
    }
}

void PKC_sliderTsEvent(PKC_item* item, FT6336G_point* tsPoint) {
    PKC_sliderP* sliderParams = (PKC_sliderP*)(item->params);
    int localValue;
    if (item->width > item->height) {
        // sliderParams->value = tsPoint->x - (item->x + EDGE_WIDTH);
        sliderParams->value+=tsPoint->dx;
        if (sliderParams->value < 0) sliderParams->value = 0;
        if (sliderParams->value > item->width-2*EDGE_WIDTH-1) sliderParams->value = item->width-2*EDGE_WIDTH-1;
        localValue = 1024*sliderParams->value/(item->width-2*EDGE_WIDTH);
        sliderParams->event = localValue;
        item->tickEnable = 1;
    } else {
        // sliderParams->value = tsPoint->y - (item->y + EDGE_WIDTH);
        sliderParams->value+=tsPoint->dy;
        if (sliderParams->value < 0) sliderParams->value = 0;
        if (sliderParams->value > item->height-2*EDGE_WIDTH-1) sliderParams->value = item->height-2*EDGE_WIDTH-1;
        localValue = 1024*(item->height-2*EDGE_WIDTH-1-sliderParams->value)/(item->height-2*EDGE_WIDTH);
        sliderParams->event = localValue;
        item->tickEnable = 1;
    }
    if (tsPoint->event & TS_PRESS_DOWN) {
        ASYS_setSweepFrequency(localValue + 200);
        ASYS_startSweep();
    } else if (tsPoint->event & TS_LIFT_UP) {
        ASYS_stop();
    } else {
        ASYS_setSweepFrequency(localValue + 200);
    }
}

void PKC_sliderTick(PKC_item* item) {
    PKC_sliderP* sliderParams = (PKC_sliderP*)(item->params);
    sliderParams->callback(sliderParams->event);
    item->tickEnable = 0;
    PKC_setRefreshItemFlag(item);
}

int PKC_sliderGetValue(PKC_item* item) {
    PKC_sliderP* sliderParams = (PKC_sliderP*)(item->params);
    return 1024*(item->height-2*EDGE_WIDTH-1-sliderParams->value)/(item->height-2*EDGE_WIDTH);
}

void PKC_sliderWriteValue(PKC_item* item, int value) {
    PKC_sliderP* sliderParams = (PKC_sliderP*)(item->params);
    if (item->width > item->height) {
        sliderParams->value = value*(item->width-2*EDGE_WIDTH)/1024;
        if (sliderParams->value < 0) sliderParams->value = 0;
        if (sliderParams->value > item->width-2*EDGE_WIDTH-1) sliderParams->value = item->width-2*EDGE_WIDTH-1;
    } else {
        sliderParams->value = item->height-2*EDGE_WIDTH - 1 - (value*(item->height-2*EDGE_WIDTH)/1024);
        if (sliderParams->value < 0) sliderParams->value = 0;
        if (sliderParams->value > item->height-2*EDGE_WIDTH-1) sliderParams->value = item->height-2*EDGE_WIDTH-1;
    }
    PKC_setRefreshItemFlag(item);
}

void PKC_sliderSetValue(PKC_item* item, int value) {
    PKC_sliderP* sliderParams = (PKC_sliderP*)(item->params);
    PKC_sliderWriteValue(item, value);
    sliderParams->event = PKC_sliderGetValue(item);
    item->tickEnable = 1;
}
