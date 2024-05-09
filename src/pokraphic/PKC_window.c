#include "PKC_window.h"

#define EDGE_WIDTH 5

void PKC_windowInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_windowP* windowParams, char* text
) {
    PKC_itemInit(item, x, y, width, height, level, &PKC_windowDraw, 0, 0);
    item->params = windowParams;
    windowParams->text = text;
}
void PKC_windowDraw(PKC_item* item) {
    PKC_windowP* windowParams = (PKC_windowP*)(item->params);
    // Color for edge drawing
    BSP_LCD_SetTextColor(item->theme->color1);
    // Horizontal bars, from top to bottom
    BSP_LCD_FillRect(item->x, item->y, item->width, EDGE_WIDTH);
    BSP_LCD_FillRect(item->x, item->y + 50, item->width, EDGE_WIDTH);
    BSP_LCD_FillRect(item->x, item->y + item->height - EDGE_WIDTH, item->width, EDGE_WIDTH);
    // Vertical bars, from left to right
    BSP_LCD_FillRect(item->x, item->y, EDGE_WIDTH, item->height);
    BSP_LCD_FillRect(item->x + item->width - EDGE_WIDTH, item->y, EDGE_WIDTH, item->height);
    // Color for title background
    BSP_LCD_SetTextColor(item->theme->color2);
    BSP_LCD_FillRect(item->x + EDGE_WIDTH, item->y + EDGE_WIDTH, item->width - 2*EDGE_WIDTH, 50 - EDGE_WIDTH);
    // Color for title
    BSP_LCD_SetTextColor(item->theme->color1);
    BSP_LCD_DisplayStringAt(item->x + item->width/2, item->y + EDGE_WIDTH + 50/2, (uint8_t*)(windowParams->text), CENTER_MODE);

    // Background
    BSP_LCD_SetTextColor(PKC_theme.backGroundColor);
    BSP_LCD_FillRect(item->x+EDGE_WIDTH, item->y+55, item->width-2*EDGE_WIDTH, item->height-60);
}
