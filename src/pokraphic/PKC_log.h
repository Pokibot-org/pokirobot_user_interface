#ifndef PKC_LOG_H
#define PKC_LOG_H

#include <stdio.h>
#include "pokraphic.h"

#define PKC_LOG_MAXLINE 20

typedef struct PKC_logLine PKC_logLine;
struct PKC_logLine {
    uint32_t color;
    char* text;
};

typedef struct PKC_logP PKC_logP;
struct PKC_logP {
    int scrollPosition;
    int scrollSpeed;
    int maxScroll;
    int maxHeight;
    int closing;
    int opening;
    int lineCount;
    PKC_logLine* lines[PKC_LOG_MAXLINE];
};

void PKC_logInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_logP* logParams
);
void PKC_logDraw(PKC_item* item);
void PKC_logTsEvent(PKC_item* item, FT6336G_point* tsPoint);
void PKC_logTick(PKC_item* item);
void PKC_logClose(PKC_item* item);
void PKC_logOpen(PKC_item* item);

void PKC_logClear(PKC_item* item);
void PKC_logAddLine(PKC_item* item, uint32_t color, char* text);


#endif