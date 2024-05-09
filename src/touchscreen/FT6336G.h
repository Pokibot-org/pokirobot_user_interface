#ifndef FT6336G_H
#define FT6336G_H

#include <stdint.h>

typedef struct {
    int x;
    int y;
    int prevx;
    int prevy;
    int dx;
    int dy;
    int dxSum;
    int dySum;
    int event;
    int timer;
    int longPress;
    int tapCondition;
    int currentItemIndex;
} FT6336G_point;

#define TS_PRESS_DOWN 0x01
#define TS_LIFT_UP 0x02
#define TS_CONTACT 0x04
#define TS_REAL_LIFT_UP 0x08
#define TS_ANY_EVENT 0xFF

#define TS_NO_EVENT_VALUE 0x00


FT6336G_point FT6336G_getP0();
FT6336G_point FT6336G_getP1();
int FT6336G_getNbrPoints();
void FT6336G_setUpdateCallback(void(*callback)(void));

void FT6336G_dma_callback();

#endif