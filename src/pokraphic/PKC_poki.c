#include "PKC_poki.h"
#include "./../audio/audio_system.h"

#include <stdlib.h>

extern uint32_t PKC_soundFlags;


void PKC_pokiSetColor(PKC_item* item, uint32_t color, int delta);
void stepColorBuffer(uint32_t* buffer, int sizex, int sizey);
void PKC_pokiSetSize(PKC_item* item, int size);

void PKC_pokiInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_pokiP* pokiParams, uint32_t* picPointer
) {
    PKC_itemInit(item, x, y, width, height, level, &PKC_pokiDraw, &PKC_pokiTsEvent, PKC_pokiTick);
    item->x = 0;
    item->y = 0;
    item->params = pokiParams;
    item->tickEnable = 1;
    pokiParams->picPointer = picPointer;
    pokiParams->speedx = 0;
    pokiParams->speedy = 0;
    pokiParams->helicopterMode = 0;
    pokiParams->helicopterModeSound = 0;
    pokiParams->newSize = width;
    pokiParams->color = 0;
    pokiParams->bounce = 1;

    pokiParams->picIndex = 0;
    pokiParams->picUpdate = 0;
}

void PKC_pokiDraw(PKC_item* item) {
    uint32_t* buffer = PKC_getCurrentBuffer();
    PKC_pokiP* pokiParams = (PKC_pokiP*)(item->params);

    if (pokiParams->picUpdate) {
        if (pokiParams->picIndex == 0) {
            PKC_pokiSetPic(item, "pics/poki_480.bin");
        } else if (pokiParams->picIndex == 1) {
            PKC_pokiSetPic(item, "pics/pokibot_2024_480.bin");
        } else if (pokiParams->picIndex == 2) {
            PKC_pokiSetPic(item, "pics/pokibot_universel_480.bin");
        }
    }


    if (pokiParams->newSize != item->width || pokiParams->picUpdate) {
        PKC_pokiSetSize(item, pokiParams->newSize);
    }
    if (pokiParams->color != item->theme->color1 || pokiParams->picUpdate) {
        pokiParams->color = item->theme->color1;
        PKC_pokiSetColor(item, pokiParams->color, 150);
    }

    pokiParams->picUpdate = 0;

    PKC_CopyBufferBlend(pokiParams->picPointer, buffer, item->x, item->y, item->width, item->height);
}

void PKC_pokiTsEvent(PKC_item* item, FT6336G_point* tsPoint) {
    PKC_pokiP* pokiParams = (PKC_pokiP*)(item->params);
    if (tsPoint->longPress) {
        if (tsPoint->x-item->width/2 >= 0 && tsPoint->y-item->height/2 >= 0 && tsPoint->x+item->width/2 <= LCD_WIDTH && tsPoint->y+item->height/2 <= LCD_HEIGHT) {
            PKC_addRefreshZoneFromItem(item);
            item->x = tsPoint->x - item->width/2;
            item->y = tsPoint->y - item->height/2;
        }

        if (tsPoint->event & TS_LIFT_UP) {
            pokiParams->speedx = ((float)tsPoint->dx)*1;
            pokiParams->speedy = ((float)tsPoint->dy)*1;
        }

        if (tsPoint->event & TS_CONTACT) {
            pokiParams->helicopterMode = 1;
        }
        if (tsPoint->event & TS_LIFT_UP) {
            pokiParams->helicopterMode = 0;
        }

        // if (tsPoint->event & TS_PRESS_DOWN) {
        //   ASYS_playFile("pok2.wav");
        //   pokiParams->speedy = -12;
        // }
    }
    if ((tsPoint->event & TS_LIFT_UP) && tsPoint->tapCondition) {
        PKC_soundFlags|=PKC_SND_POKI;
    }
}

void PKC_pokiTick(PKC_item* item) {
    PKC_pokiP* pokiParams = (PKC_pokiP*)(item->params);

    int refreshFlag = 0;

    refreshFlag |= ((int)(pokiParams->speedx) != 0 || (int)(pokiParams->speedy) != 0);

    if (refreshFlag) {
        PKC_setRefreshItemFlag(item);
        PKC_addRefreshZoneFromItem(item);
    }

    // if (TOF_event == SCROLL_RIGHT) {
    //   speedx = 8;
    // }
    // if (TOF_event == SCROLL_LEFT) {
    //   speedx = -8;
    // }
    // if (TOF_event == SCROLL_UP) {
    //   speedy = -8;
    // }
    // if (TOF_event == SCROLL_DOWN) {
    //   speedy = 8;
    // }

    item->y = item->y + (int)pokiParams->speedy;
    // pokiParams->speedy = pokiParams->speedy + 0.8;
    if (pokiParams->speedy > -1 && pokiParams->speedy < 0.5) {
        pokiParams->speedy = 0.5;
    }
    // pokiParams->speedy = 0.99*pokiParams->speedy;
    if (item->y > 479-item->height) {
        item->y = 479-item->height;
        if (pokiParams->bounce) {
            pokiParams->speedy = -pokiParams->speedy;
        } else {
            pokiParams->speedy = 0;
        }
        // if (pokiParams->speedy > 10) {
        //   printf("%02X\r\n", ASYS_playFile("exp2.wav"));
        // }
        PKC_soundFlags|=PKC_SND_POKI;
    }
    if (item->y < 0) {
        item->y = 0;
        if (pokiParams->bounce) {
            pokiParams->speedy = -pokiParams->speedy;
        } else {
            pokiParams->speedy = 0;
        }
        PKC_soundFlags|=PKC_SND_POKI;
    }
    item->x = item->x + (int)pokiParams->speedx;
    if (item->x > 799-item->width) {
        item->x = 799-item->width;
        if (pokiParams->bounce) {
            pokiParams->speedx = -pokiParams->speedx;
        } else {
            pokiParams->speedx = 0;
        }
        PKC_soundFlags|=PKC_SND_POKI;
    }
    if (item->x < 0) {
        item->x = 0;
        if (pokiParams->bounce) {
            pokiParams->speedx = -pokiParams->speedx;
        } else {
            pokiParams->speedx = 0;
        }
        PKC_soundFlags|=PKC_SND_POKI;
    }

    if (pokiParams->helicopterMode == 1) {
        if (pokiParams->helicopterModeSound == 0) {
            ASYS_repeatFile("pok2.wav", 2000, 12000, 50);
            pokiParams->helicopterModeSound = 1;
        }
    } else {
        if (pokiParams->helicopterModeSound == 1) {
            ASYS_stop(50);
            pokiParams->helicopterModeSound = 0;
        }
    }
}

void PKC_pokiSetColor(PKC_item* item, uint32_t color, int delta) {
    // 0xFF9FFC39
    PKC_pokiP* pokiParams = (PKC_pokiP*)(item->params);
    uint32_t prevColor = (pokiParams->picPointer)[item->width/2 + (item->height/3)*item->width];
    int prevR = (prevColor>>16) & 0xFF;
    int prevG = (prevColor>>8) & 0xFF;
    int prevB = (prevColor>>0) & 0xFF;
    for (int y = 0; y < item->height; y++) {
        for (int x = 0; x < item->width; x++) {
            uint32_t* address = pokiParams->picPointer + x + y*item->width;
            int r = ((*address)>>16) & 0xFF;
            int g = ((*address)>>8) & 0xFF;
            int b = ((*address)>>0) & 0xFF;
            if (abs(prevR-r) + abs(prevG-g) + abs(prevB-b) < delta) {
                *address = color;
            }
        }
    }
}

int stepColorBufferIndex = 0;
void stepColorBuffer(uint32_t* buffer, int sizex, int sizey) {
    uint32_t temp;
    for (int y = 0; y < sizey; y++) {
        for (int x = 0; x < sizex; x++) {
            if (stepColorBufferIndex == 1) {
                temp = buffer[y*sizey + x];
                // temp = (temp>>24) | temp | 0xFF000000;
                temp = (temp&0xFF0000FF) | ((temp&0x00FF0000)>>8) | ((temp&0x0000FF00)<<8);
            } else {
                temp = buffer[y*sizey + x];
                temp = (temp&0xFFFF0000) | ((temp&0x00FF)<<8) | ((temp&0xFF00)>>8);
            }
            buffer[y*sizey + x] = temp;
        }
    }
    if (stepColorBufferIndex == 1) {
        stepColorBufferIndex = 0;
    } else {
        stepColorBufferIndex = stepColorBufferIndex + 1;
    }
}

extern uint32_t poki_160x160_argb8888;

#define POKI_BASE_WIDTH 480
#define POKI_BASE_HEIGHT 480

void PKC_pokiSetSize(PKC_item* item, int size) {
    PKC_pokiP* pokiParams = (PKC_pokiP*)(item->params);
    uint32_t* buffer = pokiParams->picPointer;
    uint32_t* pokiInputBuffer = (uint32_t*)poki_160x160_argb8888;
    int xt, yt;
    uint32_t value;
    uint8_t* a = ((uint8_t*)&value) + 3;
    uint8_t* r = ((uint8_t*)&value) + 2;
    uint8_t* g = ((uint8_t*)&value) + 1;
    uint8_t* b = ((uint8_t*)&value) + 0;
    int xint, yint, xfrac, yfrac;
    uint32_t pix0, pix1, pix2, pix3;

    for (int y = 0; y < size; y++) {
        yt = 16*y*POKI_BASE_WIDTH/size;
        yfrac = yt&0xF;
        yint = yt>>4;
        for (int x = 0; x < size; x++) {
            xt = 16*x*POKI_BASE_WIDTH/size;
            value = 0;
            xfrac = xt&0xF;
            xint = xt>>4;

            pix0 = pokiInputBuffer[yint*POKI_BASE_WIDTH + xint];
            pix1 = pokiInputBuffer[yint*POKI_BASE_WIDTH + xint + 1];
            pix2 = pokiInputBuffer[(yint+1)*POKI_BASE_WIDTH + xint];
            pix3 = pokiInputBuffer[(yint+1)*POKI_BASE_WIDTH + xint + 1];

            // Magic interpollation between 4 adjacent pixels
            // (Trust me, it works, AND it is efficient)
            *b = (((16-xfrac)+(16-yfrac)) * ((pix0>>0)&0xFF) +
                ((xfrac)+(16-yfrac)) * ((pix1>>0)&0xFF) +
                ((16-xfrac)+(yfrac)) * ((pix2>>0)&0xFF) +
                ((xfrac)+(yfrac)) * ((pix3>>0)&0xFF)) / 64;
            *g = (((16-xfrac)+(16-yfrac)) * ((pix0>>8)&0xFF) +
                ((xfrac)+(16-yfrac)) * ((pix1>>8)&0xFF) +
                ((16-xfrac)+(yfrac)) * ((pix2>>8)&0xFF) +
                ((xfrac)+(yfrac)) * ((pix3>>8)&0xFF)) / 64;
            *r = (((16-xfrac)+(16-yfrac)) * ((pix0>>16)&0xFF) +
                ((xfrac)+(16-yfrac)) * ((pix1>>16)&0xFF) +
                ((16-xfrac)+(yfrac)) * ((pix2>>16)&0xFF) +
                ((xfrac)+(yfrac)) * ((pix3>>16)&0xFF)) / 64;
            *a = (((16-xfrac)+(16-yfrac)) * ((pix0>>24)&0xFF) +
                ((xfrac)+(16-yfrac)) * ((pix1>>24)&0xFF) +
                ((16-xfrac)+(yfrac)) * ((pix2>>24)&0xFF) +
                ((xfrac)+(yfrac)) * ((pix3>>24)&0xFF)) / 64;
            buffer[y*size + x] = value;
        }
    }
    item->x = item->x + (item->width-size)/2;
    item->y = item->y + (item->height-size)/2;
    if (item->x > 799-size) {
        item->x = 799-size;
    }
    if (item->y > 479-size) {
        item->y = 479-size;
    }
    if (item->x < 0) {
        item->x = 0;
    }
    if (item->y < 0) {
        item->y = 0;
    }
    item->width = size;
    item->height = size;
    // Reset color value to force color update
    pokiParams->color = 0;
}


void PKC_pokiSetPic(PKC_item* item, char* picPath) {
    PKC_loadPicInRam(picPath, (void*)poki_160x160_argb8888, 480*480*4);
    memcpy((void*)((uint32_t*)poki_160x160_argb8888 + 480*480), (void*)poki_160x160_argb8888, 480*480*sizeof(uint32_t));
    PKC_setRefreshItemFlag(item);
}