#ifndef PKC_NUMPAD_H
#define PKC_NUMPAD_H

#include <stdio.h>
#include "pokraphic.h"

#define PKC_NP_BUFFER_SIZE 20

typedef struct PKC_numPadP PKC_numPadP;
struct PKC_numPadP {
    int pressedButton;
    char buffer[PKC_NP_BUFFER_SIZE];
    int bufferLen;
    int originX;
    int originY;
    int maxWidth;
    int maxHeight;
    int zoomDirection;
    char text[PKC_NP_BUFFER_SIZE];
    void (*callback)(int mode, float value);
    int event;
};

void PKC_numPadInit(
    PKC_item* item, int level,
    PKC_numPadP* numPadParams
);
void PKC_numPadDraw(PKC_item* item);
void PKC_numPadTick(PKC_item* item);
void PKC_numPadTsEvent(PKC_item* item, FT6336G_point* tsPoint);

float PKC_numPadGetFloatValue(PKC_item* item);
int PKC_numPadGetIntValue(PKC_item* item);

void PKC_numPadOpen(PKC_item* item, float value, char* text, void (*callback)(int mode, float value));

#endif