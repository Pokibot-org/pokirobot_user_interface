#include "PKC_scrollList.h"

extern uint32_t PKC_soundFlags;

#define EDGE_WIDTH 5
#define ITEM_HEIGHT 35
#define MIN_HEIGHT 12
#define OPEN_CLOSE_SPEED 20

void PKC_scrollListInit(
    PKC_item* item, int x, int y, int width, int height, int level,
    PKC_scrollListP* scrollListParams, int itemCount, char** charArray, void (*callback)(int index)
) {
    PKC_itemInit(item, x, y, width, height, level, &PKC_scrollListDraw, &PKC_scrollListTsEvent, &PKC_scrollListTick);
    item->params = scrollListParams;
    scrollListParams->itemCount = itemCount;
    scrollListParams->charArray = charArray;
    scrollListParams->callback = callback;
    scrollListParams->index = 0;
    scrollListParams->scrollPosition = 0;
    scrollListParams->scrollSpeed = 0;
    scrollListParams->maxScroll = -1;
    scrollListParams->maxHeight = height;
    scrollListParams->closing = 0;
    scrollListParams->opening = 0;

    item->tickEnable = 0;
}

void PKC_scrollListDraw(PKC_item* item) {
    PKC_scrollListP* scrollListParams = (PKC_scrollListP*)(item->params);

    scrollListParams->maxScroll = scrollListParams->itemCount*ITEM_HEIGHT - (item->height-2*EDGE_WIDTH);
    if (scrollListParams->maxScroll <= 0) {
        scrollListParams->scrollPosition = 0;
    }

    BSP_LCD_SetTextColor(item->theme->color0);
    BSP_LCD_FillRect(item->x, item->y, item->width, item->height);

    BSP_LCD_SetTextColor(item->theme->color2);
    BSP_LCD_FillRect(item->x + EDGE_WIDTH, item->y + EDGE_WIDTH, item->width - 2*EDGE_WIDTH, item->height - 2*EDGE_WIDTH);

    BSP_LCD_SetTextColor(item->theme->color1);
    for (int i = 0; i < scrollListParams->itemCount; i++) {
        int localY = item->y + EDGE_WIDTH + 1 + ITEM_HEIGHT*i + ITEM_HEIGHT/2 - scrollListParams->scrollPosition;
        if (localY - 15 > item->y && localY + ITEM_HEIGHT/2 < item->y + item->height) {
            if (i == scrollListParams->index) {
                BSP_LCD_FillRect(item->x + EDGE_WIDTH, localY - ITEM_HEIGHT/2, item->width - 2*EDGE_WIDTH, ITEM_HEIGHT);
                BSP_LCD_SetTextColor(item->theme->color2);
                BSP_LCD_DisplayStringAt(item->x + EDGE_WIDTH + 3, localY, (uint8_t*)((scrollListParams->charArray)[i]), LEFT_CENTER_MODE);
                BSP_LCD_SetTextColor(item->theme->color1);
            } else {
                BSP_LCD_DisplayStringAt(item->x + EDGE_WIDTH + 3, localY, (uint8_t*)((scrollListParams->charArray)[i]), LEFT_CENTER_MODE);
            }
        }
    }

    // Small scrollbar on the right side
    if (scrollListParams->maxScroll > 0) {
        int localY = item->y + EDGE_WIDTH + (item->height - EDGE_WIDTH*7) * scrollListParams->scrollPosition / scrollListParams->maxScroll;
        BSP_LCD_SetTextColor(item->theme->color2);
        BSP_LCD_FillRect(item->x + item->width - EDGE_WIDTH + 1, localY, EDGE_WIDTH - 2, EDGE_WIDTH*5);
    }

}

void PKC_scrollListTsEvent(PKC_item* item, FT6336G_point* tsPoint) {
    PKC_scrollListP* scrollListParams = (PKC_scrollListP*)(item->params);
    if (scrollListParams->maxScroll > 0) {
        scrollListParams->scrollPosition -= tsPoint->dy;
        if (scrollListParams->scrollPosition < 0) {
            scrollListParams->scrollPosition = 0;
            scrollListParams->scrollSpeed = 0;
        } else if (scrollListParams->scrollPosition > scrollListParams->maxScroll) {
            scrollListParams->scrollPosition = scrollListParams->maxScroll;
            scrollListParams->scrollSpeed = 0;
        } else if ((tsPoint->event & TS_LIFT_UP) && !tsPoint->tapCondition) {
            scrollListParams->scrollSpeed = -5*tsPoint->dy;
        }
    }
    if ((tsPoint->event & TS_LIFT_UP) && tsPoint->tapCondition) {
        int newIndex = (tsPoint->y - (item->y + EDGE_WIDTH) + scrollListParams->scrollPosition) / ITEM_HEIGHT;
        if (newIndex < scrollListParams->itemCount) {
            scrollListParams->index = newIndex;
            scrollListParams->callback(newIndex);
            PKC_soundFlags |= PKC_SND_BLIP00;
        }
    }
    if (!(tsPoint->event & TS_LIFT_UP)) {
        scrollListParams->scrollSpeed = 0;
    }
    item->tickEnable = 1;
}

void PKC_scrollListTick(PKC_item* item) {
    PKC_scrollListP* scrollListParams = (PKC_scrollListP*)(item->params);
    scrollListParams->scrollPosition = scrollListParams->scrollPosition + scrollListParams->scrollSpeed;
    scrollListParams->scrollSpeed = 99*scrollListParams->scrollSpeed / 100;
    if (scrollListParams->scrollPosition < 0) {
        scrollListParams->scrollPosition = 0;
        scrollListParams->scrollSpeed = 0;
    } else if (scrollListParams->scrollPosition > scrollListParams->maxScroll) {
        scrollListParams->scrollPosition = scrollListParams->maxScroll;
        scrollListParams->scrollSpeed = 0;
    }
    if (scrollListParams->opening) {
        item->height = item->height + OPEN_CLOSE_SPEED;
        if (item->height >= scrollListParams->maxHeight) {
            item->height = scrollListParams->maxHeight;
            scrollListParams->opening = 0;
        }
    }
    if (scrollListParams->closing) {
        item->height = item->height - OPEN_CLOSE_SPEED;
        if (item->height <= MIN_HEIGHT) {
            item->drawEnable = 0;
            item->height = MIN_HEIGHT;
            scrollListParams->closing = 0;
        }
    }
    if (scrollListParams->scrollSpeed == 0 && !scrollListParams->opening && !scrollListParams->closing) {
        item->tickEnable = 0;
    } else {
        item->tickEnable = 1;
    }
    PKC_setRefreshItemFlag(item);
}

void PKC_scrollListClose(PKC_item* item) {
    PKC_scrollListP* scrollListParams = (PKC_scrollListP*)(item->params);
    scrollListParams->closing = 1;
    item->tickEnable = 1;
}

void PKC_scrollListOpen(PKC_item* item) {
    PKC_scrollListP* scrollListParams = (PKC_scrollListP*)(item->params);
    item->height = MIN_HEIGHT;
    item->drawEnable = 1;
    scrollListParams->opening = 1;
    item->tickEnable = 1;
}

void PKC_scrollListSetIndex(PKC_item* item, int index) {
    PKC_scrollListP* scrollListParams = (PKC_scrollListP*)(item->params);
    if (index >= 0 && index < scrollListParams->itemCount) {
        scrollListParams->index = index;
        scrollListParams->callback(index);
    }
}