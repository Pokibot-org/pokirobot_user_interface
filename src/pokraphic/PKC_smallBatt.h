#ifndef PKC_SMALLBATT_H
#define PKC_SMALLBATT_H

#include "pokraphic.h"

#include <stdio.h>

typedef struct PKC_smallBattP PKC_smallBattP;
struct PKC_smallBattP {
    int fromage;
};

void PKC_smallBattInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_smallBattP* smallBattParams
);
void PKC_smallBattDraw(PKC_item* item);

#endif