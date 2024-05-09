#ifndef VLC53L5CX_APP_H
#define VLC53L5CX_APP_H

#include "vl53l5cx_api.h"
#include "vl53l5cx_plugin_motion_indicator.h"

#define TOF_NO_EVENT 0
#define SCROLL_UP 1
#define SCROLL_DOWN 2
#define SCROLL_RIGHT 3
#define SCROLL_LEFT 4

int TOF_init();
int TOF_updateData();

#endif