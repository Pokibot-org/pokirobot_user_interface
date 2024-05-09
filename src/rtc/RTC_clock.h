#ifndef RTC_CLOCK_H
#define RTC_CLOCK_H

#include <stdio.h>

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t days;
    uint8_t weekdays;
    uint8_t months;
    uint8_t years;
} RTC_time;

void RTC_init();
void RTC_update();
void RTC_set(RTC_time newTime);
void RTC_dma_callback();

#endif