#ifndef PKC_BUTTON_H
#define PKC_BUTTON_H

#include <stdio.h>
#include "pokraphic.h"

#define PKC_BUTTON_PRESS 0x01
#define PKC_BUTTON_RELEASE 0x02


typedef struct PKC_buttonP PKC_buttonP;
struct PKC_buttonP {
    char* text;
    int value;
    int event;
    void (*callback)(int event);
};

void PKC_buttonInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_buttonP* buttonParams, char* text, void (*callback)(int event)
);
void PKC_buttonDraw(PKC_item* item);
void PKC_buttonTsEvent(PKC_item* item, FT6336G_point* tsPoint);
void PKC_buttonTick(PKC_item* item);

void PKC_switchInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_buttonP* buttonParams, char* text, void (*callback)(int event)
);
void PKC_switchDraw(PKC_item* item);
void PKC_switchTsEvent(PKC_item* item, FT6336G_point* tsPoint);
void PKC_switchTick(PKC_item* item);
void PKC_switchSetValue(PKC_item* item, int v);

#endif