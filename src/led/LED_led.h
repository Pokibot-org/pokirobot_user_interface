#ifndef LED_LED_H
#define LED_LED_H

#include <stdio.h>

#define LED_TOP_LEFT 0
#define LED_TOP_MIDDLE 1
#define LED_TOP_RIGHT 2
#define LED_BOTTOM_LEFT 3
#define LED_BOTTOM_MIDDLE 4
#define LED_BOTTOM_RIGHT 5

int LED_init();
void LED_setColor(int led_index, int red, int green, int blue);
void LED_setBlink(int led_index, int red_blink, int green_blink, int blue_blink);
void LED_test();
void LED_update();
void LED_setBrightness(uint8_t value);
void LED_setBlinkRate(int period_ms);

#endif