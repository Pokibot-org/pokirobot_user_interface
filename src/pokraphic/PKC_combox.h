#ifndef PKC_COMBOX_H
#define PKC_COMBOX_H

#include <stdio.h>
#include "pokraphic.h"
#include "PKC_scrollList.h"

typedef struct PKC_comboxP PKC_comboxP;
struct PKC_comboxP {
    int index;
    int deployed;
    PKC_item* scrollList;
    char* text;
    void (*callback)(int index);
};

void PKC_comboxInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_comboxP* comboxParams, PKC_item* scrollList, void (*callback)(int index)
);
void PKC_comboxDraw(PKC_item* item);
void PKC_comboxTsEvent(PKC_item* item, FT6336G_point* tsPoint);
void PKC_comboxScrollListCallback(PKC_item* item, int index);

#endif