#ifndef PKC_THEMES_H
#define PKC_THEMES_H

#include <stdio.h>

typedef struct PKC_themes PKC_themes;

struct PKC_themes {
    uint32_t color0;
    uint32_t color1;
    uint32_t color2;
    uint32_t backGroundColor;
    char blipName[8*2+1];
};

extern PKC_themes PKC_theme;

extern PKC_themes PKC_themeRed;
extern PKC_themes PKC_themeGreen;
extern PKC_themes PKC_themeBlue;

#define PKC_DEFAULT_MAIN_COLOR 0xFF9FFC39
#define PKC_DEFAULT_BACKGROUND_COLOR LCD_COLOR_DARKGRAY

#endif