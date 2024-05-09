#ifndef POKRAPHIC_ITEM_H
#define POKRAPHIC_ITEM_H

#include <stdio.h>

#include "pokraphic.h"
#include "PKC_themes.h"

#include "./../touchscreen/FT6336G.h"

typedef struct PKC_item PKC_item;

// Pokigraphic item
struct PKC_item {
    // Position
    int x;
    int y;
    // Size
    int width;
    int height;

    int drawEnable;
    int tickEnable;
    int needRefresh;

    int level;
    PKC_themes* theme;

    void (*draw)(PKC_item* item);
    void (*tsEvent)(PKC_item* item, FT6336G_point* tsPoint);
    void (*tick)(PKC_item* item);

    void* params;
};

void PKC_itemInit(PKC_item* item, int x, int y, int width, int height, int level,
    void (*draw)(PKC_item* item), void (*tsEvent)(PKC_item* item, FT6336G_point* tsPoint), void (*tick)(PKC_item* item));

#endif