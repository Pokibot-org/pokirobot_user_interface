#include "PKC_combox.h"

extern uint32_t PKC_soundFlags;

#define EDGE_WIDTH 5

void PKC_comboxInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_comboxP* comboxParams, PKC_item* scrollList, void (*callback)(int index)
) {
    PKC_scrollListP* scrollListParams = (PKC_scrollListP*)(scrollList->params);
    PKC_itemInit(item, x, y, width, height, level, &PKC_comboxDraw, &PKC_comboxTsEvent, 0);
    item->params = comboxParams;
    comboxParams->text = scrollListParams->charArray[0];
    comboxParams->callback = callback;
    comboxParams->index = 0;
    comboxParams->deployed = 0;
    comboxParams->scrollList = scrollList;
    scrollList->drawEnable = 0;
}

void PKC_comboxDeploy(PKC_item* item) {
    PKC_comboxP* comboxParams = (PKC_comboxP*)(item->params);
    comboxParams->deployed = 1;
    PKC_scrollListOpen(comboxParams->scrollList);
}

void PKC_comboxClose(PKC_item* item) {
    PKC_comboxP* comboxParams = (PKC_comboxP*)(item->params);
    comboxParams->deployed = 0;
    PKC_scrollListClose(comboxParams->scrollList);
}

void PKC_comboxDraw(PKC_item* item) {
    PKC_comboxP* comboxParams = (PKC_comboxP*)(item->params);

    BSP_LCD_SetTextColor(item->theme->color0);
    BSP_LCD_FillRect(item->x, item->y, item->width, item->height);

    BSP_LCD_SetTextColor(item->theme->color2);
    BSP_LCD_FillRect(item->x + EDGE_WIDTH, item->y + EDGE_WIDTH, item->width - 2*EDGE_WIDTH, item->height - 2*EDGE_WIDTH);

    BSP_LCD_SetTextColor(item->theme->color1);
    BSP_LCD_DisplayStringAt(item->x + EDGE_WIDTH + 3, item->y + item->height/2, (uint8_t*)(comboxParams->text), LEFT_CENTER_MODE);

    int localX = item->x + item->width - EDGE_WIDTH - 5 - 20/2;
    int localY = item->y + item->height/2 - 5;
    for (int i = 0; i < 10; i++) {
        BSP_LCD_DrawHLine(localX - (10-i), localY + i, 20-2*i);
    }
}

void PKC_comboxTsEvent(PKC_item* item, FT6336G_point* tsPoint) {
    PKC_comboxP* comboxParams = (PKC_comboxP*)(item->params);
    if ((tsPoint->event & TS_LIFT_UP) && tsPoint->tapCondition) {
        if (comboxParams->deployed) {
            PKC_comboxClose(item);
        } else {
            PKC_comboxDeploy(item);
        }
        PKC_soundFlags |= PKC_SND_BLIP00;
    }
}

void PKC_comboxScrollListCallback(PKC_item* item, int index) {
    PKC_comboxP* comboxParams = (PKC_comboxP*)(item->params);
    PKC_scrollListP* scrollListParams = (PKC_scrollListP*)(comboxParams->scrollList->params);
    PKC_comboxClose(item);
    comboxParams->text = scrollListParams->charArray[index];
    comboxParams->index = index;
    comboxParams->callback(index);
}