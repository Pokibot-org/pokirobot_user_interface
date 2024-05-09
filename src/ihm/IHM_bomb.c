#include "IHM_bomb.h"

#include "./../pokraphic/pokraphic.h"
#include "./../pokraphic/PKC_button.h"
#include "./../pokraphic/PKC_segments.h"


#include "./../audio/audio_system.h"

PKC_item segmentsScore;
PKC_segmentsP segmentsScoreParams;

void (*IHM_bombCloseCallback)();

int IHM_bombIsOpen = 0;

void IHM_segmentsTsEvent(PKC_item* item, FT6336G_point* tsPoint) {
    if (tsPoint->event & TS_PRESS_DOWN) {
        PKC_segmentsSetTarget(item, 0);
        ASYS_playFile("alla.wav", 110);
    }
}

void IHM_bombInit(void (*fun)()) {
    IHM_bombCloseCallback = fun;
    PKC_segmentsInit(
        &segmentsScore, 400-700/2, 240-300/2, 700, 300, 10,
        &segmentsScoreParams, 3
    );
    segmentsScore.tickEnable = 0;
}

void IHM_bombOpen(int value) {
    segmentsScore.tsEvent = IHM_segmentsTsEvent;
    PKC_addItem(&segmentsScore);
    segmentsScore.tickEnable = 1;
    PKC_segmentsSetTarget(&segmentsScore, value);
    PKC_segmentsSetValue(&segmentsScore, value);
    IHM_bombIsOpen = 1;
}
void IHM_bombClose() {
    segmentsScore.tsEvent = 0;
    PKC_removeItem(&segmentsScore);
    segmentsScore.tickEnable = 0;
    IHM_bombIsOpen = 0;
}

int bombTick = 0;
void IHM_bombTick() {
    if (IHM_bombIsOpen) {
        if (bombTick + 1 >= 50) {
            bombTick = 0;
        } else {
            bombTick = bombTick + 1;
        }
        if (bombTick == 0) {
            if (segmentsScoreParams.value > 0 && segmentsScoreParams.target != 0) {
                PKC_segmentsSetValue(&segmentsScore, segmentsScoreParams.value - 1);
                segmentsScoreParams.target = segmentsScoreParams.value;
                ASYS_playFile("blips/bl05.wav", 100);
            }
        }
        if (segmentsScoreParams.value == 0) {
            ASYS_playFile("exp2.wav", 120);
            IHM_bombCloseCallback();
        }
    }
}