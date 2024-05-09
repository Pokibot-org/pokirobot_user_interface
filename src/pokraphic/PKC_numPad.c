#include "PKC_numPad.h"

#define EDGE_WIDTH 5

char buttonText[] = "7\0008\0009\000<\0004\0005\0006\000.\0001\0002\0003\0000";

extern uint32_t PKC_soundFlags;

void PKC_numPadSetText(PKC_item* item, char* text) {
    PKC_numPadP* numPadParams = (PKC_numPadP*)(item->params);
    strncpy(numPadParams->text, text, PKC_NP_BUFFER_SIZE-1);
    // If strlen(text) is greater than PKC_NP_BUFFER_SIZE-1,
    // there will be no trailing null char.
    // Thus forcing last byte of array to 0 to ensure that
    // the string is correctly terminated.
    numPadParams->text[PKC_NP_BUFFER_SIZE-1] = 0;
}

void PKC_numPadInit(
    PKC_item* item, int level,
    PKC_numPadP* numPadParams
) {
    int x = 400-350/2;
    int y = 240-400/2;
    int width = 349;
    int height = 400;
    PKC_itemInit(item, x, y, width, height, level, &PKC_numPadDraw, &PKC_numPadTsEvent, &PKC_numPadTick);
    item->params = numPadParams;
    item->tickEnable = 1;
    numPadParams->callback = 0;
    numPadParams->pressedButton = -1;
    numPadParams->bufferLen = 0;
    numPadParams->buffer[0] = 0;
    numPadParams->zoomDirection = -2;
    numPadParams->originX = x;
    numPadParams->originY = y;
    numPadParams->maxWidth = width;
    numPadParams->maxHeight = height;
    numPadParams->text[0] = 0; // Null string
    numPadParams->event = -1;
}

void PKC_numPadDraw(PKC_item* item) {
    PKC_numPadP* numPadParams = (PKC_numPadP*)(item->params);

    int cellHeight = (item->height-EDGE_WIDTH) / 5;
    int cellWidth = (item->width-EDGE_WIDTH) / 4;

    BSP_LCD_SetTextColor(item->theme->color2);
    BSP_LCD_FillRect(item->x, item->y, item->width, cellHeight);

    BSP_LCD_SetTextColor(item->theme->color1);
    BSP_LCD_FillRect(item->x, item->y + cellHeight, item->width, item->height - cellHeight);

    BSP_LCD_SetTextColor(item->theme->color0);
    for (int i = 0; i < 6; i++) {
        BSP_LCD_FillRect(item->x, item->y + i*cellHeight, item->width, EDGE_WIDTH);
    }
    BSP_LCD_FillRect(item->x, item->y + cellHeight/3, item->width, EDGE_WIDTH);
    for (int i = 0; i < 5; i++) {
        if ((i&3) == 0) {
            BSP_LCD_FillRect(item->x + i*cellWidth, item->y, EDGE_WIDTH, item->height);
        } else if ((i&1) == 0) {
            BSP_LCD_FillRect(item->x + i*cellWidth, item->y + cellHeight, EDGE_WIDTH, item->height-cellHeight);
        } else {
            BSP_LCD_FillRect(item->x + i*cellWidth, item->y + cellHeight, EDGE_WIDTH, item->height-2*cellHeight);
        }
    }

    BSP_LCD_SetTextColor(item->theme->color2);
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 4; x++) {
            if (numPadParams->pressedButton == y*4+x) {
                BSP_LCD_FillRect(item->x + x*cellWidth + EDGE_WIDTH, item->y + y*cellHeight + cellHeight + EDGE_WIDTH, cellWidth-EDGE_WIDTH, cellHeight-EDGE_WIDTH);
                BSP_LCD_SetTextColor(item->theme->color1);
                BSP_LCD_DisplayStringAt(item->x + cellWidth/2 + EDGE_WIDTH/2 + x*cellWidth, item->y + cellHeight/2 + EDGE_WIDTH/2 + (1+y)*cellHeight, (uint8_t*)buttonText + (x+y*4)*2, CENTER_MODE);
                BSP_LCD_SetTextColor(item->theme->color2);
            } else {
                BSP_LCD_DisplayStringAt(item->x + cellWidth/2 + EDGE_WIDTH/2 + x*cellWidth, item->y + cellHeight/2 + EDGE_WIDTH/2 + (1+y)*cellHeight, (uint8_t*)buttonText + (x+y*4)*2, CENTER_MODE);
            }
        }
    }
    if (numPadParams->pressedButton == 12) {
        BSP_LCD_FillRect(item->x + EDGE_WIDTH, item->y + 4*cellHeight + EDGE_WIDTH, 2*cellWidth-EDGE_WIDTH, cellHeight-EDGE_WIDTH);
        BSP_LCD_SetTextColor(item->theme->color1);
        BSP_LCD_DisplayStringAt(item->x + cellWidth + EDGE_WIDTH/2, item->y + item->height - cellHeight/2 - EDGE_WIDTH/2, (uint8_t*)"Annuler", CENTER_MODE);
        BSP_LCD_SetTextColor(item->theme->color2);
    } else {
        BSP_LCD_DisplayStringAt(item->x + cellWidth + EDGE_WIDTH/2, item->y + item->height - cellHeight/2 - EDGE_WIDTH/2, (uint8_t*)"Annuler", CENTER_MODE);
    }
    if (numPadParams->pressedButton == 13) {
        BSP_LCD_FillRect(item->x + 2*cellWidth + EDGE_WIDTH, item->y + 4*cellHeight + EDGE_WIDTH, 2*cellWidth-EDGE_WIDTH, cellHeight-EDGE_WIDTH);
        BSP_LCD_SetTextColor(item->theme->color1);
        BSP_LCD_DisplayStringAt(item->x + 3*cellWidth + EDGE_WIDTH/2, item->y + item->height - cellHeight/2 - EDGE_WIDTH/2, (uint8_t*)"Valider", CENTER_MODE);
        BSP_LCD_SetTextColor(item->theme->color2);
    } else {
        BSP_LCD_DisplayStringAt(item->x + 3*cellWidth + EDGE_WIDTH/2, item->y + item->height - cellHeight/2 - EDGE_WIDTH/2, (uint8_t*)"Valider", CENTER_MODE);
    }

    BSP_LCD_SetTextColor(item->theme->color1);
    BSP_LCD_DisplayStringAt(item->x + item->width - 2*EDGE_WIDTH - numPadParams->bufferLen*17, item->y + 3*cellHeight/4, (uint8_t*)(numPadParams->buffer), LEFT_CENTER_MODE);

    BSP_LCD_SetTextColor(item->theme->color1);
    BSP_LCD_DisplayStringAt(item->x + item->width/2, item->y + EDGE_WIDTH + 11, (uint8_t*)(numPadParams->text), CENTER_MODE);

}

void PKC_numPadTick(PKC_item* item) {
    PKC_numPadP* numPadParams = (PKC_numPadP*)(item->params);
    if (numPadParams->zoomDirection == 1) {
        item->width += numPadParams->maxWidth/10;
        item->height += numPadParams->maxHeight/10;
        if (item->width >= numPadParams->maxWidth || item->height >= numPadParams->maxHeight) {
            item->width = numPadParams->maxWidth;
            item->height = numPadParams->maxHeight;
            item->x = numPadParams->originX;
            item->y = numPadParams->originY;
            numPadParams->zoomDirection = 2;
        } else {
            item->x = numPadParams->originX + (numPadParams->maxWidth-item->width) / 2;
            item->y = numPadParams->originY + (numPadParams->maxHeight-item->height) / 2;
        }
        PKC_setRefreshItemFlag(item);
    } else if (numPadParams->zoomDirection == -1) {
        // Refresh befor width/height modifications
        // Refresh the maximum size, because some text can overpass the expected boudaries
        PKC_addRefreshZone(numPadParams->originX, numPadParams->originY, numPadParams->maxWidth, numPadParams->maxHeight);
        PKC_setRefreshItemFlag(item);
        // Reduce item size
        item->width -= numPadParams->maxWidth/10;
        item->height -= numPadParams->maxHeight/10;
        if (item->width <= 10 || item->height <= 10) {
            item->width = 10;
            item->height = 10;
            numPadParams->zoomDirection = -2;
            PKC_removeItem(item);
        } else {
            item->x = numPadParams->originX + (numPadParams->maxWidth-item->width) / 2;
            item->y = numPadParams->originY + (numPadParams->maxHeight-item->height) / 2;
        }

    }
    if (numPadParams->event >= 0) {
        numPadParams->callback(numPadParams->event, PKC_numPadGetIntValue(item));
        numPadParams->event = -1;
    }
}

void PKC_numPadTsEvent(PKC_item* item, FT6336G_point* tsPoint) {
    PKC_numPadP* numPadParams = (PKC_numPadP*)(item->params);
    int cellHeight = (item->height-EDGE_WIDTH) / 5;
    int cellWidth = (item->width-EDGE_WIDTH) / 4;

    int rx = tsPoint->x - item->x;
    int ry = tsPoint->y - item->y;

    if (ry > cellHeight) {
        ry = ry - cellHeight;
        if (tsPoint->event & TS_PRESS_DOWN) {
            PKC_soundFlags |= PKC_SND_BLIP00;
        }
        if ((tsPoint->event & TS_REAL_LIFT_UP) && tsPoint->tapCondition) {
            if (numPadParams->pressedButton == 3) {
                if (numPadParams->bufferLen > 0) {
                    numPadParams->buffer[numPadParams->bufferLen-1] = 0;
                    numPadParams->bufferLen--;
                }
            } else if (numPadParams->pressedButton == 12) {
                numPadParams->zoomDirection = -1;
                numPadParams->event = 0;
            } else if (numPadParams->pressedButton == 13) {
                numPadParams->zoomDirection = -1;
                numPadParams->event = 1;
            } else {
                if (numPadParams->bufferLen < PKC_NP_BUFFER_SIZE-1) {
                    numPadParams->buffer[numPadParams->bufferLen] = buttonText[numPadParams->pressedButton*2];
                    numPadParams->buffer[numPadParams->bufferLen+1] = 0;
                    numPadParams->bufferLen++;
                }
            }
            numPadParams->pressedButton = -1;
            PKC_setRefreshItemFlag(item);
        } else if (tsPoint->event & TS_LIFT_UP) {
            numPadParams->pressedButton = -1;
            PKC_setRefreshItemFlag(item);
        } else {
            if (ry < 3*cellHeight) {
                numPadParams->pressedButton = (ry/cellHeight) * 4 + rx/cellWidth;
                PKC_setRefreshItemFlag(item);
            } else {
                numPadParams->pressedButton = rx/(2*cellWidth) + 12;
                PKC_setRefreshItemFlag(item);
            }
            if (numPadParams->pressedButton == 3 && tsPoint->longPress) {
                numPadParams->buffer[0] = 0;
                numPadParams->bufferLen = 0;
            }
        }
    } else {
        numPadParams->pressedButton = -1;
        PKC_setRefreshItemFlag(item);
    }
}

float PKC_numPadGetFloatValue(PKC_item* item) {
    PKC_numPadP* numPadParams = (PKC_numPadP*)(item->params);
    float v;
    sscanf(numPadParams->buffer, "%f", &v);
    return v;
}

int PKC_numPadGetIntValue(PKC_item* item) {
    PKC_numPadP* numPadParams = (PKC_numPadP*)(item->params);
    int v;
    sscanf(numPadParams->buffer, "%d", &v);
    return v;
}

void PKC_numPadOpen(PKC_item* item, float value, char* text, void (*callback)(int mode, float value)) {
    PKC_numPadP* numPadParams = (PKC_numPadP*)(item->params);
    if (numPadParams->zoomDirection == -2) {
        numPadParams->callback = callback;
        if (value == 0) {
            numPadParams->buffer[0] = 0;
            numPadParams->bufferLen = 0;
        } else {
            numPadParams->bufferLen = sprintf(numPadParams->buffer, "%d", (int)value);
        }
        item->width = 10;
        item->height = 10;
        item->x = numPadParams->originX + (numPadParams->maxWidth-item->width) / 2;
        item->y = numPadParams->originY + (numPadParams->maxHeight-item->height) / 2;
        PKC_addItem(item);
        numPadParams->zoomDirection = 1;
        PKC_numPadSetText(item, text);
    }
}