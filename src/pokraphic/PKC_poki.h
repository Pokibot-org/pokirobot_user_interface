#ifndef PKC_POKI_H
#define PKC_POKI_H

#include <stdio.h>

#include "pokraphic.h"

typedef struct PKC_pokiP PKC_pokiP;

struct PKC_pokiP {
    uint32_t* picPointer;
    int touchMoveFlag;
    float speedx;
    float speedy;
    int helicopterMode;
    int helicopterModeSound;
    int newSize;
    int bounce;
    uint32_t color;

    int picIndex;
    int picUpdate;
};

void PKC_pokiInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_pokiP* pokiParams, uint32_t* picPointer
);
void PKC_pokiDraw(PKC_item* item);
void PKC_pokiTsEvent(PKC_item* item, FT6336G_point* tsPoint);
void PKC_pokiTick(PKC_item* item);

void PKC_pokiSetPic(PKC_item* item, char* picPath);

#endif