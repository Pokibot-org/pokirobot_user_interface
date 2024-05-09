#ifndef PKC_WINDOW_H
#define PKC_WINDOW_H

#include <stdio.h>
#include "pokraphic.h"

typedef struct PKC_windowP PKC_windowP;
struct PKC_windowP {
    char* text;
};

void PKC_windowInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_windowP* windowParams, char* text
);
void PKC_windowDraw(PKC_item* item);

#endif