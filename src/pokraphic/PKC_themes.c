#include "PKC_themes.h"

#include "stm32469i_discovery_lcd.h"

PKC_themes PKC_theme = {
    LCD_COLOR_BLUE, LCD_COLOR_LIGHTBLUE, LCD_COLOR_DARKBLUE,
    LCD_COLOR_DARKGRAY, "blips/bl00.wav"
};

PKC_themes PKC_themeRed = {
    0xFFAA0000,
    0xFFFF0000,
    0xFF550000,
    0x00000000,
    "blips/bl04.wav"
};

PKC_themes PKC_themeGreen = {
    0xFF00AA00,
    0xFF00FF00,
    0xFF005500,
    0x00000000,
    "blips/bl04.wav"
};

PKC_themes PKC_themeBlue = {
    0xFF0000AA,
    0xFF0000FF,
    0xFF000055,
    0x00000000,
    "blips/bl04.wav"
};