#ifndef PKC_SLIDER_H
#define PKC_SLIDER_H

#include <stdio.h>
#include "pokraphic.h"

typedef struct PKC_sliderP PKC_sliderP;
struct PKC_sliderP {
    int value;
    int event;
    void (*callback)(int value); // Value is given between 0 and 1023
};

void PKC_sliderInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_sliderP* sliderParams, void (*callback)(int value)
);
void PKC_sliderDraw(PKC_item* item);
void PKC_sliderTsEvent(PKC_item* item, FT6336G_point* tsPoint);
void PKC_sliderTick(PKC_item* item);

// Get value from 0 to 1023
int PKC_sliderGetValue(PKC_item* item);

// Both functions do the same thing,
// except PKC_sliderWriteValue does not call the callback function
void PKC_sliderWriteValue(PKC_item* item, int value);
void PKC_sliderSetValue(PKC_item* item, int value);

#endif