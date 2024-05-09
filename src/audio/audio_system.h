#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include "audio.h"
#include "cs43l22.h"

#include <stdio.h>


#define ASYS_OK 0
#define ASYS_ERROR 1
#define ASYS_UNKNOWN_FFORMAT 2
#define ASYS_UNKNOWN_AFORMAT 3
#define ASYS_AFORMAT_NOT_SUPPORTED 4
#define ASYS_FILE_ERROR 5
#define ASYS_COOLDOWN 6
#define ASYS_CODEC_ERROR 10
#define ASYS_DMA_ERROR 11

#define ASYS_OUT_MAIN OUTPUT_DEVICE_HEADPHONE
#define ASYS_OUT_AUX OUTPUT_DEVICE_SPEAKER
#define ASYS_OUT_NONE 0

void ASYS_init();

int ASYS_playFile(char* path, int priority);
int ASYS_repeatFile(char* path, int start, int stop, int priority);
int ASYS_stop();

void ASYS_setSweepFrequency(int frequency);
void ASYS_startSweep();

// Volume input from 0 to 255
void ASYS_setVolume(int vol);
// Mode between ASYS_OUT_MAIN, ASYS_OUT_AUX, ASYS_OUT_NONE
void ASYS_setOutputMode(int mode);

#endif