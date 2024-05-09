#ifndef PKC_SEGMENTS_H
#define PKC_SEGMENTS_H

#include <stdio.h>
#include "pokraphic.h"

typedef struct PKC_segmentsP PKC_segmentsP;
struct PKC_segmentsP {
    int nbrDigits;
    int minDigits;
    int value;
    int target;
};

void PKC_segmentsInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_segmentsP* segmentsParams, int minDigits
);
void PKC_segmentsDraw(PKC_item* item);
void PKC_segmentsTick(PKC_item* item);

void PKC_segmentsSetValue(PKC_item* item, int value);
void PKC_segmentsSetTarget(PKC_item* item, int value);

#endif