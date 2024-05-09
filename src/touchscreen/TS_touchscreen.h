#ifndef TS_TOUCHSCREEN_H
#define TS_TOUCHSCREEN_H

#include "FT6336G.h"

typedef struct {
    FT6336G_point p0;
    FT6336G_point p1;
} TS_event;


#endif;