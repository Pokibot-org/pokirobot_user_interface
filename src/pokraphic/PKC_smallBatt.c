#include "PKC_smallBatt.h"

#include "./../battery/BAT_battery.h"

#include <string.h>

void PKC_smallBattInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_smallBattP* smallBattParams
) {
    PKC_itemInit(item, x, y, width, height, level, &PKC_smallBattDraw, 0, 0);
    item->params = smallBattParams;
    smallBattParams->fromage = 0;
}
void PKC_smallBattDraw(PKC_item* item) {
    // PKC_smallBattP* smallBattParams = (PKC_smallBattP*)(item->params);

    // Background
    BSP_LCD_SetTextColor(PKC_theme.backGroundColor);
    BSP_LCD_FillRect(item->x, item->y, item->width, item->height);

    // Text
    BSP_LCD_SetTextColor(BAT_getColorIndicator());
    char buffer[20];
    int v = BAT_getVoltage();
    int c = BAT_getCurrent();
    sprintf(buffer, "V:%d.%03d", v/1000, v%1000);
    BSP_LCD_DisplayStringAt(item->x + 5, item->y + item->height/4, (uint8_t*)(buffer), LEFT_CENTER_MODE);
    sprintf(buffer, "I:%d.%03d", c/1000, c%1000);
    BSP_LCD_DisplayStringAt(item->x + 5, item->y + 3*item->height/4, (uint8_t*)(buffer), LEFT_CENTER_MODE);

    // Always refresh
    PKC_setRefreshItemFlag(item);
    item->needRefresh = 10;
}
