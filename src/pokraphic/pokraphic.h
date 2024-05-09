#ifndef POKIGRAPHIC_H
#define POKIGRAPHIC_H

#include <stdio.h>
#include "stm32469i_discovery.h"
#include "stm32469i_discovery_lcd.h"

#include "PKC_themes.h"
#include "pokraphic_item.h"

#define LAYER0_ADDRESS (LCD_FB_START_ADDRESS)


#define LCD_WIDTH 800
#define LCD_HEIGHT 480
#define LCD_BUFFER_SIZE (LCD_WIDTH*LCD_HEIGHT*4)

#define PKC_SND_BLIP00 ((uint32_t)(1<<0))
#define PKC_SND_POKI ((uint32_t)(1<<1))
#define PKC_SND_SPAWN ((uint32_t)(1<<8))
#define PKC_SND_SONIC ((uint32_t)(1<<9))

// I dont know why the fuck I need to put this line, but it will not compile whithout it
typedef struct PKC_item PKC_item;

void PKC_setColorTheme(uint32_t color);

int PKC_init();
void PKC_tick();
void PKC_forceAllRefresh();
void PKC_setRGBMode(int value);
void PKC_refresh();
uint32_t* PKC_getCurrentBuffer();

void PKC_tsEvent();


void PKC_addItem(PKC_item* item);
void PKC_removeItem(PKC_item* item);

void PKC_CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize);
void PKC_CopyBufferBlend(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize);
void PKC_drawBuffer(uint32_t *pSrc, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize);

typedef struct PKC_refreshZone PKC_refreshZone;
struct PKC_refreshZone {
    int x;
    int y;
    int width;
    int height;
    int timeToLive;
};
void PKC_addRefreshZone(int x, int y, int width, int height);
void PKC_addRefreshZoneFromItem(PKC_item* item);
void PKC_addRefreshZoneAll();

void PKC_setRefreshItemFlag(PKC_item* item);


void PKC_loadPicInRam(char* picPath, void* ramPointer, int bytesize);

#endif