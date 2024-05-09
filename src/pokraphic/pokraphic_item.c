#include "pokraphic_item.h"

extern PKC_themes PKC_theme;

void PKC_itemInit(PKC_item* item, int x, int y, int width, int height, int level,
    void (*draw)(PKC_item* item), void (*tsEvent)(PKC_item* item, FT6336G_point* tsPoint), void (*tick)(PKC_item* item)) {
    item->x = x;
    item->y = y;
    item->width = width;
    item->height = height;
    item->level = level;
    item->theme = &PKC_theme;
    item->draw = draw;
    item->tsEvent = tsEvent;
    item->tick = tick;
    item->drawEnable = 1;
    item->tickEnable = 0;
    item->needRefresh = 1;
}