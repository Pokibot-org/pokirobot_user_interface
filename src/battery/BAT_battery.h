#ifndef BATTERY_H
#define BATTERY_H

#include <stdio.h>

typedef struct BAT_dataPoint BAT_dataPoint;
struct BAT_dataPoint {
    int voltage; // In mV
    int current; // In mA
};

int BAT_init();
void BAT_update();

// Return value in mV
int BAT_getVoltage();
// Return value in mA
int BAT_getCurrent();
int BAT_isEmergencyStopReleased();
// Input current in mA
void BAT_setEmergencyStopDetectionCurrentValue(int c);

uint32_t BAT_getColorIndicator();

void BAT_dma_callback();

int BAT_lipoAtWarningVoltage();
int BAT_lipoAtCriticalVoltage();

#endif