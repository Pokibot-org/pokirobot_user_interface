#ifndef PKC_SCROLLLIST_H
#define PKC_SCROLLLIST_H

#include <stdio.h>
#include "pokraphic.h"

typedef struct PKC_scrollListP PKC_scrollListP;
struct PKC_scrollListP {
    int index;
    int scrollPosition;
    int scrollSpeed;
    int maxScroll;
    int itemCount;
    int maxHeight;
    int closing;
    int opening;
    char** charArray;
    void (*callback)(int index);
};

void PKC_scrollListInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_scrollListP* scrollListParams, int itemCount, char** charArray, void (*callback)(int index)
);
void PKC_scrollListDraw(PKC_item* item);
void PKC_scrollListTsEvent(PKC_item* item, FT6336G_point* tsPoint);
void PKC_scrollListTick(PKC_item* item);
void PKC_scrollListClose(PKC_item* item);
void PKC_scrollListOpen(PKC_item* item);
void PKC_scrollListSetIndex(PKC_item* item, int index);

#endif