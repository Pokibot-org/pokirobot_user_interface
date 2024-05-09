#include "PKC_log.h"

#include <string.h>
#include <stdlib.h>

extern uint32_t PKC_soundFlags;

#define EDGE_WIDTH 5
#define ITEM_HEIGHT 25
#define MIN_HEIGHT 12
#define OPEN_CLOSE_SPEED 20

void PKC_logUpdateMaxScroll(PKC_item* item) {
    PKC_logP* logParams = (PKC_logP*)(item->params);
    logParams->maxScroll = logParams->lineCount*ITEM_HEIGHT - (item->height-2*EDGE_WIDTH);
    if (logParams->maxScroll <= 0) {
        logParams->scrollPosition = 0;
    }
}

void PKC_logInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_logP* logParams
) {
    PKC_itemInit(item, x, y, width, height, level, &PKC_logDraw, &PKC_logTsEvent, &PKC_logTick);
    item->params = logParams;
    logParams->scrollPosition = 0;
    logParams->scrollSpeed = 0;
    logParams->maxScroll = -1;
    logParams->maxHeight = height;
    logParams->closing = 0;
    logParams->opening = 0;
    logParams->lineCount = 0;

    item->tickEnable = 0;
}

void PKC_logDraw(PKC_item* item) {
    PKC_logP* logParams = (PKC_logP*)(item->params);

    PKC_logUpdateMaxScroll(item);

    // Fill background
    BSP_LCD_SetTextColor(item->theme->color2);
    BSP_LCD_FillRect(item->x + EDGE_WIDTH, item->y + EDGE_WIDTH, item->width - 2*EDGE_WIDTH, item->height - 2*EDGE_WIDTH);

    // Text lines
    for (int i = 0; i < logParams->lineCount; i++) {
        int localY = EDGE_WIDTH + 1 + ITEM_HEIGHT*i + ITEM_HEIGHT/2 - logParams->scrollPosition;
        if (localY > ITEM_HEIGHT/2 && localY + ITEM_HEIGHT/2 < item->height) {
            BSP_LCD_SetTextColor((logParams->lines)[i]->color);
            BSP_LCD_DisplayStringAt(item->x + EDGE_WIDTH + 3, item->y + localY, (uint8_t*)((logParams->lines)[i]->text), LEFT_CENTER_MODE);
        }
    }

    // Edges
    BSP_LCD_SetTextColor(item->theme->color1);
    BSP_LCD_FillRect(item->x, item->y, item->width, EDGE_WIDTH);
    BSP_LCD_FillRect(item->x, item->y + item->height - EDGE_WIDTH, item->width, EDGE_WIDTH);
    BSP_LCD_FillRect(item->x, item->y, EDGE_WIDTH, item->height);
    BSP_LCD_FillRect(item->x + item->width - EDGE_WIDTH, item->y, EDGE_WIDTH, item->height);

    // Small scrollbar on the right side
    if (logParams->maxScroll > 0) {
        int localY = item->y + EDGE_WIDTH + (item->height - EDGE_WIDTH*7) * logParams->scrollPosition / logParams->maxScroll;
        BSP_LCD_SetTextColor(item->theme->color2);
        BSP_LCD_FillRect(item->x + item->width - EDGE_WIDTH + 1, localY, EDGE_WIDTH - 2, EDGE_WIDTH*5);
    }
}

void PKC_logTsEvent(PKC_item* item, FT6336G_point* tsPoint) {
    PKC_logP* logParams = (PKC_logP*)(item->params);
    if (logParams->maxScroll > 0) {
        logParams->scrollPosition -= tsPoint->dy;
        if (logParams->scrollPosition < 0) {
            logParams->scrollPosition = 0;
            logParams->scrollSpeed = 0;
        } else if (logParams->scrollPosition > logParams->maxScroll) {
            logParams->scrollPosition = logParams->maxScroll;
            logParams->scrollSpeed = 0;
        } else if ((tsPoint->event & TS_LIFT_UP) && !tsPoint->tapCondition) {
            logParams->scrollSpeed = -5*tsPoint->dy;
        }
    }
    if (!(tsPoint->event & TS_LIFT_UP)) {
        logParams->scrollSpeed = 0;
    }
    item->tickEnable = 1;
}

void PKC_logTick(PKC_item* item) {
    PKC_logP* logParams = (PKC_logP*)(item->params);
    logParams->scrollPosition = logParams->scrollPosition + logParams->scrollSpeed;
    logParams->scrollSpeed = 99*logParams->scrollSpeed / 100;
    if (logParams->scrollPosition < 0) {
        logParams->scrollPosition = 0;
        logParams->scrollSpeed = 0;
    } else if (logParams->scrollPosition > logParams->maxScroll) {
        logParams->scrollPosition = logParams->maxScroll;
        logParams->scrollSpeed = 0;
    }
    if (logParams->opening) {
        item->height = item->height + OPEN_CLOSE_SPEED;
        if (item->height >= logParams->maxHeight) {
            item->height = logParams->maxHeight;
            logParams->opening = 0;
        }
    }
    if (logParams->closing) {
        item->height = item->height - OPEN_CLOSE_SPEED;
        if (item->height <= MIN_HEIGHT) {
            item->drawEnable = 0;
            item->height = MIN_HEIGHT;
            logParams->closing = 0;
        }
    }
    if (logParams->scrollSpeed == 0 && !logParams->opening && !logParams->closing) {
        item->tickEnable = 0;
    } else {
        item->tickEnable = 1;
    }
    PKC_setRefreshItemFlag(item);
}

void PKC_logClose(PKC_item* item) {
    PKC_logP* logParams = (PKC_logP*)(item->params);
    logParams->closing = 1;
    item->tickEnable = 1;
}

void PKC_logOpen(PKC_item* item) {
    PKC_logP* logParams = (PKC_logP*)(item->params);
    item->height = MIN_HEIGHT;
    item->drawEnable = 1;
    logParams->opening = 1;
    item->tickEnable = 1;
}

void PKC_logFreeLine(PKC_logLine* line) {
    free(line->text);
    free(line);
}

void PKC_logClear(PKC_item* item) {
    PKC_logP* logParams = (PKC_logP*)(item->params);
    for (int i = 0; i < logParams->lineCount; i++) {
        PKC_logFreeLine(logParams->lines[i]);
    }
    logParams->lineCount = 0;
}

void PKC_logAddLine(PKC_item* item, uint32_t color, char* text) {
    PKC_logP* logParams = (PKC_logP*)(item->params);
    if (logParams->lineCount >= PKC_LOG_MAXLINE) {
        // Delete oldest line, and move all lines with 1 index
        PKC_logFreeLine(logParams->lines[0]);
        for (int i = 0; i < PKC_LOG_MAXLINE - 1; i++) {
            logParams->lines[i] = logParams->lines[i+1];
        }
        logParams->lineCount = PKC_LOG_MAXLINE - 1;
    }
    PKC_logLine* line = malloc(sizeof(PKC_logLine));
    line->color = color;
    line->text = malloc((strlen(text)+1)*sizeof(char));
    strcpy(line->text, text);
    logParams->lines[logParams->lineCount] = line;
    logParams->lineCount = logParams->lineCount + 1;

    // Push scroll to the end
    PKC_logUpdateMaxScroll(item);
    // logParams->scrollPosition = logParams->maxScroll;
    logParams->scrollSpeed += 10;
    item->tickEnable = 1;
}