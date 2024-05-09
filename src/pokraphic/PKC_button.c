#include "PKC_button.h"

extern uint32_t PKC_soundFlags;

#define EDGE_WIDTH 5

void PKC_buttonInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_buttonP* buttonParams, char* text, void (*callback)(int event)
) {
    PKC_itemInit(item, x, y, width, height, level, &PKC_buttonDraw, &PKC_buttonTsEvent, &PKC_buttonTick);
    item->params = buttonParams;
    buttonParams->value = 0;
    buttonParams->event = 0;
    buttonParams->text = text;
    buttonParams->callback = callback;
}

void PKC_buttonDraw(PKC_item* item) {
    PKC_buttonP* buttonParams = (PKC_buttonP*)(item->params);
    if (buttonParams->value == 0) {
        BSP_LCD_SetTextColor(item->theme->color0);
        BSP_LCD_FillRect(item->x, item->y, item->width, item->height);

        BSP_LCD_SetTextColor(item->theme->color1);
        for (int i = 0; i < 4; i++) {
            BSP_LCD_DrawHLine(item->x, item->y + i, item->width - i);
            BSP_LCD_DrawVLine(item->x + i, item->y, item->height - i);
        }
        BSP_LCD_SetTextColor(item->theme->color2);
        for (int i = 0; i < 4; i++) {
            BSP_LCD_DrawVLine(item->x + item->width - 1 - i, item->y + i, item->height - i);
            BSP_LCD_DrawHLine(item->x + i, item->y + item->height - 1 - i, item->width - i);
        }
        BSP_LCD_SetTextColor(item->theme->color1);
        BSP_LCD_DisplayStringAt(item->x + item->width/2, item->y + item->height/2, (uint8_t*)(buttonParams->text), CENTER_MODE);
    } else {
        BSP_LCD_SetTextColor(item->theme->color0);
        BSP_LCD_FillRect(item->x, item->y, item->width, item->height);

        BSP_LCD_SetTextColor(item->theme->color2);
        for (int i = 0; i < 4; i++) {
            BSP_LCD_DrawHLine(item->x, item->y + i, item->width - i);
            BSP_LCD_DrawVLine(item->x + i, item->y, item->height - i);
        }
        BSP_LCD_SetTextColor(item->theme->color1);
        for (int i = 0; i < 4; i++) {
            BSP_LCD_DrawVLine(item->x + item->width - 1 - i, item->y + i, item->height - i);
            BSP_LCD_DrawHLine(item->x + i, item->y + item->height - 1 - i, item->width - i);
        }
        BSP_LCD_SetTextColor(item->theme->color2);
        BSP_LCD_DisplayStringAt(item->x + item->width/2, item->y + item->height/2, (uint8_t*)(buttonParams->text), CENTER_MODE);
    }
}

void PKC_buttonTsEvent(PKC_item* item, FT6336G_point* tsPoint) {
    PKC_buttonP* buttonParams = (PKC_buttonP*)(item->params);
    if (tsPoint->event & TS_PRESS_DOWN) {
        PKC_soundFlags |= PKC_SND_BLIP00;
        buttonParams->value = 1;
        buttonParams->event |= PKC_BUTTON_PRESS;
        item->tickEnable = 1;
    } else if ((tsPoint->event & TS_REAL_LIFT_UP) && tsPoint->tapCondition) {
        buttonParams->value = 0;
        buttonParams->event |= PKC_BUTTON_RELEASE;
        item->tickEnable = 1;
    } else if (tsPoint->event & TS_LIFT_UP) {
        buttonParams->value = 0;
        item->tickEnable = 1;
    }
}

void PKC_buttonTick(PKC_item* item) {
    PKC_buttonP* buttonParams = (PKC_buttonP*)(item->params);
    if (buttonParams->event) {
        buttonParams->callback(buttonParams->event);
        buttonParams->event = 0;
    }
    item->tickEnable = 0;
    PKC_setRefreshItemFlag(item);
}


void PKC_switchInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_buttonP* buttonParams, char* text, void (*callback)(int event)
) {
    PKC_itemInit(item, x, y, width, height, level, &PKC_switchDraw, &PKC_switchTsEvent, &PKC_switchTick);
    item->params = buttonParams;
    buttonParams->value = 0;
    buttonParams->event = 0;
    buttonParams->text = text;
    buttonParams->callback = callback;
}

void PKC_switchDraw(PKC_item* item) {
    PKC_buttonP* buttonParams = (PKC_buttonP*)(item->params);
    BSP_LCD_SetTextColor(item->theme->color0);
    BSP_LCD_FillRect(item->x, item->y, item->width, item->height);
    if (buttonParams->value == 0) {
        BSP_LCD_SetTextColor(item->theme->color2);
        BSP_LCD_FillRect(item->x + EDGE_WIDTH, item->y + EDGE_WIDTH, item->width - 2*EDGE_WIDTH, item->height - 2*EDGE_WIDTH);
        BSP_LCD_SetTextColor(item->theme->color1);
        BSP_LCD_DisplayStringAt(item->x + item->width/2, item->y + item->height/2, (uint8_t*)(buttonParams->text), CENTER_MODE);
    } else {
        BSP_LCD_SetTextColor(item->theme->color1);
        BSP_LCD_FillRect(item->x + EDGE_WIDTH, item->y + EDGE_WIDTH, item->width - 2*EDGE_WIDTH, item->height - 2*EDGE_WIDTH);
        BSP_LCD_SetTextColor(item->theme->color2);
        BSP_LCD_DisplayStringAt(item->x + item->width/2, item->y + item->height/2, (uint8_t*)(buttonParams->text), CENTER_MODE);
    }
}

void PKC_switchTsEvent(PKC_item* item, FT6336G_point* tsPoint) {
    PKC_buttonP* buttonParams = (PKC_buttonP*)(item->params);
    if ((tsPoint->event & TS_REAL_LIFT_UP) && tsPoint->tapCondition) {
        PKC_soundFlags |= PKC_SND_BLIP00;
        buttonParams->value = 1 - buttonParams->value;
        buttonParams->event = buttonParams->value;
        item->tickEnable = 1;
    }
}

void PKC_switchTick(PKC_item* item) {
    PKC_buttonP* buttonParams = (PKC_buttonP*)(item->params);
    buttonParams->callback(buttonParams->event);
    buttonParams->event = 0;
    item->tickEnable = 0;
    PKC_setRefreshItemFlag(item);
}

void PKC_switchSetValue(PKC_item* item, int v) {
    PKC_buttonP* buttonParams = (PKC_buttonP*)(item->params);
    buttonParams->value = v;
    buttonParams->event = buttonParams->value;
    item->tickEnable = 1;
}