#include "PKC_segments.h"

#include <stdlib.h>

extern uint32_t PKC_soundFlags;

#define EDGE_WIDTH 5

void PKC_segmentsInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_segmentsP* segmentsParams, int minDigits
) {
    PKC_itemInit(item, x, y, width, height, level, PKC_segmentsDraw, 0, PKC_segmentsTick);
    item->params = segmentsParams;
    segmentsParams->value = 0;
    segmentsParams->nbrDigits = minDigits;
    segmentsParams->minDigits = minDigits;
}

void drawVSegment(int x, int y, int w, int h) {
    for (int i = 0; i < w; i++) {
        BSP_LCD_DrawVLine(x - w/2 + i, y + abs(w/2-i), h - 2*abs(w/2-i));
    }
    // BSP_LCD_FillRect(x-w/2, y, w, h);
}
void drawHSegment(int x, int y, int w, int h) {
    for (int i = 0; i < h; i++) {
        BSP_LCD_DrawHLine(x + abs(h/2-i), y - h/2 + i, w - 2*abs(h/2-i));
    }
    // BSP_LCD_FillRect(x, y-h/2, w, h);
}
void drawSevenSegments(PKC_item* item, int x, int y, int w, int h, int value) {
    int spaceSize = w/30;
    int segSize = w/5;
    if ((segSize&1) == 0) {
        segSize--;
    }
    int vsize = (h-4*spaceSize-segSize)/2;
    int hsize = w-segSize-2*spaceSize;
    int segLUT[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

    // A
    if ((segLUT[value]>>0)&1) {
        BSP_LCD_SetTextColor(item->theme->color1);
        drawHSegment(x+segSize/2+spaceSize, y+segSize/2, hsize, segSize);
    } else {
        BSP_LCD_SetTextColor(item->theme->color2);
    }
    // B
    if ((segLUT[value]>>1)&1) {
        BSP_LCD_SetTextColor(item->theme->color1);
        drawVSegment(x+segSize/2+2*spaceSize+hsize-1, y+segSize/2+spaceSize, segSize, vsize);
    } else {
        BSP_LCD_SetTextColor(item->theme->color2);
    }
    // C
    if ((segLUT[value]>>2)&1) {
        BSP_LCD_SetTextColor(item->theme->color1);
        drawVSegment(x+segSize/2+2*spaceSize+hsize-1, y+segSize/2+3*spaceSize+vsize-1, segSize, vsize);
    } else {
        BSP_LCD_SetTextColor(item->theme->color2);
    }
    // D
    if ((segLUT[value]>>3)&1) {
        BSP_LCD_SetTextColor(item->theme->color1);
        drawHSegment(x+segSize/2+spaceSize, y+segSize/2+4*spaceSize+2*vsize-2, hsize, segSize);
    } else {
        BSP_LCD_SetTextColor(item->theme->color2);
    }
    // E
    if ((segLUT[value]>>4)&1) {
        BSP_LCD_SetTextColor(item->theme->color1);
        drawVSegment(x+segSize/2, y+segSize/2+3*spaceSize+vsize-1, segSize, vsize);
    } else {
        BSP_LCD_SetTextColor(item->theme->color2);
    }
    // F
    if ((segLUT[value]>>5)&1) {
        BSP_LCD_SetTextColor(item->theme->color1);
        drawVSegment(x+segSize/2, y+segSize/2+spaceSize, segSize, vsize);
    } else {
        BSP_LCD_SetTextColor(item->theme->color2);
    }
    // G
    if ((segLUT[value]>>6)&1) {
        BSP_LCD_SetTextColor(item->theme->color1);
        drawHSegment(x+segSize/2+spaceSize, y+segSize/2+2*spaceSize+vsize-1, hsize, segSize);
    } else {
        BSP_LCD_SetTextColor(item->theme->color2);
    }
}

void PKC_segmentsDraw(PKC_item* item) {
    PKC_segmentsP* segmentsParams = (PKC_segmentsP*)(item->params);

    // Background
    BSP_LCD_SetTextColor(PKC_theme.backGroundColor);
    BSP_LCD_FillRect(item->x, item->y, item->width, item->height);

    int space = item->width/30;
    int digitWidth = item->width / segmentsParams->nbrDigits - space;
    int digitHeight = item->height-space;
    int value = segmentsParams->value;
    for (int i = segmentsParams->nbrDigits-1; i >= 0; i--) {
        drawSevenSegments(item, item->x+space/2+i*(digitWidth+space), item->y+space/2, digitWidth, digitHeight, value%10);
        value = value / 10;
    }
}

void PKC_segmentsTick(PKC_item* item) {
    PKC_segmentsP* segmentsParams = (PKC_segmentsP*)(item->params);
    if (segmentsParams->value != segmentsParams->target) {
        int delta = (segmentsParams->target-segmentsParams->value)/10;
        if (delta == 0) {
            if (segmentsParams->target-segmentsParams->value > 0) {
                delta = 1;
            } else {
                delta = -1;
            }
        }
        PKC_segmentsSetValue(item, segmentsParams->value + delta);
        PKC_soundFlags|=PKC_SND_SONIC;
    } else {
        item->tickEnable = 0;
    }
}

void PKC_segmentsSetValue(PKC_item* item, int value) {
    PKC_segmentsP* segmentsParams = (PKC_segmentsP*)(item->params);
    segmentsParams->value = value;
    int i = 0;
    while(value) {
        i++;
        value = value/10;
    }
    if (i < segmentsParams->minDigits) {
        segmentsParams->nbrDigits = segmentsParams->minDigits;
    } else {
        segmentsParams->nbrDigits = i;
    }
    PKC_setRefreshItemFlag(item);
}

void PKC_segmentsSetTarget(PKC_item* item, int value) {
    PKC_segmentsP* segmentsParams = (PKC_segmentsP*)(item->params);
    segmentsParams->target = value;
    item->tickEnable = 1;
}