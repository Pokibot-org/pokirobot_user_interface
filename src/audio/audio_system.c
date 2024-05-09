#include "audio_system.h"

#include "stm32469i_discovery.h"

#include "fatfs.h"

#include <string.h>



#include "./../pokraphic/pokraphic.h"
extern uint32_t PKC_soundFlags;


#define PLAY_HEADER_BASE     0x2C
#define PLAY_BUFF_SIZE       4096
extern SAI_HandleTypeDef     hsai_BlockA1;
extern DMA_HandleTypeDef     hdma_sai1_a;
AUDIO_DrvTypeDef            *audio_drv = &cs43l22_drv;
uint8_t                      PlayBuff[PLAY_BUFF_SIZE*2];

int readBufferIndex = 0;
uint32_t bytePlaybackPosition;

FIL fil;
FSIZE_t fsize;
UINT read_bytes;

int playingStep = 0;
int loopMode = 0;
int sweepMode = 0;
int startByte, stopByte;
int playHeader = PLAY_HEADER_BASE;

int currentPriority = -1;

#define SWEEP_SAMPLE_FREQ AUDIO_FREQUENCY_44K
int sweepFrequency = 440;
void ASYS_setSweepBuffer(uint8_t* buffer);


inline void ASYS_initSilenceBuffer() {
    for (int i = 0; i < PLAY_BUFF_SIZE; i++) {
        PlayBuff[i+PLAY_BUFF_SIZE] = 0;
    }
}
inline void ASYS_sendSilenceBuffer() {
    HAL_SAI_Transmit_DMA(&hsai_BlockA1, PlayBuff + PLAY_BUFF_SIZE, PLAY_BUFF_SIZE/2);
}


int updateConfiguration(int frequency, int channel) {

    // Configure PLL
    // Considering that we have a 1MHz clock as input for PLLI2S
    // If this is not the case, user must adapt PLL parameters
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI_PLLI2S;
    if (frequency == AUDIO_FREQUENCY_11K ||
        frequency == AUDIO_FREQUENCY_22K ||
        frequency == AUDIO_FREQUENCY_44K) {
        // Target frequency: 11289600 Hz
        PeriphClkInitStruct.PLLI2S.PLLI2SN = 429;
        PeriphClkInitStruct.PLLI2S.PLLI2SQ = 2;
        PeriphClkInitStruct.PLLI2SDivQ = 19;
    } else if (frequency == AUDIO_FREQUENCY_48K ||
                frequency == AUDIO_FREQUENCY_96K ||
                frequency == AUDIO_FREQUENCY_192K) {
        // Target frequency: 49152000 Hz
        PeriphClkInitStruct.PLLI2S.PLLI2SN = 344;
        PeriphClkInitStruct.PLLI2S.PLLI2SQ = 7;
        PeriphClkInitStruct.PLLI2SDivQ = 1;
    } else if (frequency == AUDIO_FREQUENCY_8K ||
                frequency == AUDIO_FREQUENCY_16K ||
                frequency == AUDIO_FREQUENCY_32K) {
        // Target frequency: 8192000 Hz
        PeriphClkInitStruct.PLLI2S.PLLI2SN = 213;
        PeriphClkInitStruct.PLLI2S.PLLI2SQ = 2;
        PeriphClkInitStruct.PLLI2SDivQ = 13;
    } else {
        return 1;
    }
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        return 1;
    }

    // Configure SAI peripheral
    hsai_BlockA1.Instance = SAI1_Block_A;
    hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_TX;
    hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
    hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLE;
    hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
    hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;
    hsai_BlockA1.Init.ClockSource = SAI_CLKSOURCE_PLLI2S;
    hsai_BlockA1.Init.AudioFrequency = frequency;
    hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
    if (channel == 2) {
        hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
    } else if (channel == 1) {
        hsai_BlockA1.Init.MonoStereoMode = SAI_MONOMODE;
    } else {
        return 1;
    }
    hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
    hsai_BlockA1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
    if (HAL_SAI_InitProtocol(&hsai_BlockA1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, 2) != HAL_OK) {
        return 1;
    }

    // Configure DMA
    hdma_sai1_a.Instance = DMA2_Stream1;
    hdma_sai1_a.Init.Channel = DMA_CHANNEL_0;
    hdma_sai1_a.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sai1_a.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_a.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_a.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_sai1_a.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_sai1_a.Init.Mode = DMA_NORMAL;
    hdma_sai1_a.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sai1_a.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sai1_a.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sai1_a.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_sai1_a.Init.PeriphBurst = DMA_PBURST_SINGLE;
    if (HAL_DMA_Init(&hdma_sai1_a) != HAL_OK) {
        return 1;
    }

    // Configure audio codec
    audio_drv->Stop(AUDIO_I2C_ADDRESS, 0);
    if(0 != audio_drv->Init(AUDIO_I2C_ADDRESS, OUTPUT_DEVICE_HEADPHONE, 80, frequency)) { // OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_BOTH, OUTPUT_DEVICE_HEADPHONE
        return 1;
    }
    HAL_Delay(20);
    return 0;
}

void ASYS_init() {
    updateConfiguration(AUDIO_FREQUENCY_44K, 1);
    audio_drv->Init(AUDIO_I2C_ADDRESS, OUTPUT_DEVICE_HEADPHONE, 80, 0); // OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_BOTH, OUTPUT_DEVICE_HEADPHONE
    audio_drv->Play(AUDIO_I2C_ADDRESS, NULL, 0);
    ASYS_stop(1000);
}

// Accepting any file in Wav format, with 1 or 2 channels, with sample frequency in list bellow:
// 192K, 96K, 48K, 44K, 32K, 22K, 16K, 11K, 8K
int ASYS_startFile(char* path) {

    sweepMode = 0;

    if (playingStep == 1) {
        ASYS_stop(1000);
    }
    HAL_GPIO_WritePin(AUDIO_RST_GPIO_Port, AUDIO_RST_Pin, 1);
    playingStep = 1;

    int r = f_open(&fil, path, FA_READ);
    if (r != FR_OK) {
        // Cannot open file
        return ASYS_FILE_ERROR;
    }
    fsize = f_size(&fil);

    uint32_t headerBuffer[32];
    uint8_t* headerBuffer8 = (uint8_t*)headerBuffer;
    f_read(&fil, (void*)headerBuffer, 32*4, &read_bytes);

    // Check RIFF and WAVE file format
    if (headerBuffer[0] != 0x46464952 || headerBuffer[2] != 0x45564157) {
        f_close(&fil);
        // Unkown file format
        return ASYS_UNKNOWN_FFORMAT;
    }

    // Check format of first block
    // Squalala: this is a random comment. Be happy to read it. Yeah, its 2AM, and im not sure if im able to write code.
    if (headerBuffer[3] != 0x20746D66 || headerBuffer[4] != 0x00000010 ||
    (headerBuffer[5]&0x0000FFFF) != 0x0001) {
        f_close(&fil);
        return ASYS_UNKNOWN_AFORMAT;
    }
    playHeader = PLAY_HEADER_BASE;
    while (playHeader < 31*4 &&
        headerBuffer8[playHeader - 8 + 0] != 0x64 &&
        headerBuffer8[playHeader - 8 + 1] != 0x61 &&
        headerBuffer8[playHeader - 8 + 2] != 0x74 &&
        headerBuffer8[playHeader - 8 + 3] != 0x61
    ) {
        //printf("%04x %08x\r\n", playHeader, headerBuffer[playHeader/4 - 2]);
        playHeader+=1;
    }
    // printf("%02x %08x\r\n", playHeader, headerBuffer[playHeader/4 - 2]);
    if (playHeader >= 31*4) {
        f_close(&fil);
        return ASYS_UNKNOWN_AFORMAT;
    }

    // Update PLL, SAI and DMA with file configuration
    // if (updateConfiguration(headerBuffer[6], headerBuffer[5]>>16) != ASYS_OK) {
    //     f_close(&fil);
    //     return ASYS_AFORMAT_NOT_SUPPORTED;
    // }

    uint32_t dataSize = ((uint32_t)headerBuffer8[playHeader - 4 + 0]) |
                                    (headerBuffer8[playHeader - 4 + 1]<<8) |
                                    (headerBuffer8[playHeader - 4 + 2]<<16) |
                                    (headerBuffer8[playHeader - 4 + 3]<<24);

    if (loopMode) {
        if (stopByte <= 0 || dataSize + playHeader < stopByte) {
            stopByte = dataSize + playHeader;
        } else {
            stopByte = stopByte;
        }
    } else {
        stopByte = dataSize + playHeader;
    }

    f_lseek(&fil, playHeader+startByte);
    f_read(&fil, (void*)PlayBuff, PLAY_BUFF_SIZE, &read_bytes);

    bytePlaybackPosition = PLAY_BUFF_SIZE+startByte;

    if (0 != audio_drv->Play(AUDIO_I2C_ADDRESS, NULL, 0)) {
        f_close(&fil);
        return ASYS_CODEC_ERROR;
    }

    // if (HAL_OK != HAL_SAI_Transmit_DMA(&hsai_BlockA1, PlayBuff, PLAY_BUFF_SIZE/2)) {
    //     f_close(&fil);
    //     audio_drv->Stop(AUDIO_I2C_ADDRESS, 0);
    //     return ASYS_DMA_ERROR;
    // }

    // f_read(&fil, (void*)PlayBuff+PLAY_BUFF_SIZE, PLAY_BUFF_SIZE, &read_bytes);
    // bytePlaybackPosition = PLAY_BUFF_SIZE+startByte;
    readBufferIndex = 0;

    return ASYS_OK;
}


// priority = 0 is the lowest priority
// priority = 255 is the strongest priority
int ASYS_playFile(char* path, int priority) {
    // printf("%s %d %d\r\n", path, priority, currentPriority);
    if (currentPriority <= priority) {
        loopMode = 0;
        startByte = 0;
        currentPriority = priority;
        return ASYS_startFile(path);
    } else {
        return 42;
    }
}
// int ASYS_playFile(char* path) {
//     return ASYS_playFilePriority(path, 128);
// }


int ASYS_repeatFile(char* path, int start, int stop, int priority) {
    if (currentPriority < priority) {
        startByte = start;
        stopByte = stop;
        loopMode = 1;
        currentPriority = priority;
        return ASYS_startFile(path);
    } else {
        return 42;
    }
}

// int ASYS_stop() {
//     HAL_SAI_DMAStop(&hsai_BlockA1);
//     audio_drv->Stop(AUDIO_I2C_ADDRESS, 0);
//     f_close(&fil);
//     HAL_GPIO_WritePin(AUDIO_RST_GPIO_Port, AUDIO_RST_Pin, 0);
//     playingStep = 0;
//     readBufferIndex = -1;
//     return 0;
// }
int ASYS_stop(int priority) {
    // printf("Stop %d %d\r\n", priority, currentPriority);
    if (currentPriority <= priority) {
        f_close(&fil);
        ASYS_initSilenceBuffer();
        ASYS_sendSilenceBuffer();
        readBufferIndex = -2;
        playingStep = 0;
        currentPriority = -1;
        return 0;
    } else {
        return 1;
    }
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai) {
    uint8_t* writeBuffer;
    if (readBufferIndex == -3) {
        ASYS_sendSilenceBuffer();
        return;
    } else if (readBufferIndex == -2) {
        ASYS_sendSilenceBuffer();
        readBufferIndex = -3;
        // HAL_GPIO_WritePin(AUDIO_RST_GPIO_Port, AUDIO_RST_Pin, 0);
        return;
    } else if (readBufferIndex == -1) {
        ASYS_stop(currentPriority);
        return;
    } else if (readBufferIndex == 0) {
        HAL_SAI_Transmit_DMA(&hsai_BlockA1, PlayBuff, read_bytes/2);
        writeBuffer = PlayBuff + PLAY_BUFF_SIZE;
        readBufferIndex = 1;
    } else {
        HAL_SAI_Transmit_DMA(&hsai_BlockA1, PlayBuff + PLAY_BUFF_SIZE, read_bytes/2);
        writeBuffer = PlayBuff;
        readBufferIndex = 0;
    }

    if (!sweepMode) {
        // Normal mode: playing from file in SD card
        if (bytePlaybackPosition+PLAY_BUFF_SIZE < stopByte) {
            f_read(&fil, (void*)(writeBuffer), PLAY_BUFF_SIZE, &read_bytes);
        } else if (bytePlaybackPosition < stopByte) {
            f_read(&fil, (void*)(writeBuffer), stopByte - bytePlaybackPosition, &read_bytes);
        } else {
            read_bytes = 0;
        }
        bytePlaybackPosition = bytePlaybackPosition + read_bytes;
        if (read_bytes != PLAY_BUFF_SIZE) {
            if (!loopMode) {
                readBufferIndex = -1;
            } else {
                f_lseek(&fil, playHeader+startByte);
                bytePlaybackPosition = startByte;
            }
        }
    } else {
        // Sweep mode: playing from mathematically generated buffer
        ASYS_setSweepBuffer(writeBuffer);
    }
}

void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai) {
}



void ASYS_setSweepFrequency(int frequency) {
    sweepFrequency = frequency;
}

void ASYS_setSweepBuffer(uint8_t* buffer) {
    int samplePerPeriod = SWEEP_SAMPLE_FREQ/sweepFrequency;
    read_bytes = 2*(((PLAY_BUFF_SIZE/4)/samplePerPeriod)*samplePerPeriod);
    int16_t audioData;
    int position;
    for (int i = 0; i < read_bytes/2; i++) {
        position = (1024*i/samplePerPeriod) & 0x3FF;
        // if (position & 0x200) {
        //     audioData = 0x4000;
        // } else {
        //     audioData = -0x4000;
        // }
        if (position < 0x200) {
            audioData = 0x4000*(position-0x100)/0x100;
        } else {
            audioData = -0x4000*(position-0x300)/0x100;
        }
        buffer[i*2] = audioData&0xFF;
        buffer[i*2+1] = (audioData>>8)&0xFF;
    }
}

void ASYS_startSweep() {
    sweepMode = 1;

    if (playingStep == 1) {
        ASYS_stop(currentPriority);
    }
    HAL_GPIO_WritePin(AUDIO_RST_GPIO_Port, AUDIO_RST_Pin, 1);
    playingStep = 1;

    // Update PLL, SAI and DMA
    // updateConfiguration(SWEEP_SAMPLE_FREQ, 1);

    // if (0 != audio_drv->Play(AUDIO_I2C_ADDRESS, NULL, 0)) {
    //     return;
    // }

    ASYS_setSweepBuffer(PlayBuff);
    readBufferIndex = 0;

    // HAL_SAI_Transmit_DMA(&hsai_BlockA1, PlayBuff, read_bytes/2);
}


// Volume input from 0 to 255
void ASYS_setVolume(int vol) {
    audio_drv->SetVolume(AUDIO_I2C_ADDRESS, vol);
    PKC_soundFlags |= PKC_SND_BLIP00;
}

// Mode between ASYS_OUT_MAIN, ASYS_OUT_AUX, ASYS_OUT_NONE
void ASYS_setOutputMode(int mode) {
    if (mode == ASYS_OUT_MAIN || mode == ASYS_OUT_AUX) {
        audio_drv->SetMute(AUDIO_I2C_ADDRESS, AUDIO_MUTE_OFF);
        audio_drv->SetOutputMode(AUDIO_I2C_ADDRESS, mode);
    } else {
        audio_drv->SetMute(AUDIO_I2C_ADDRESS, AUDIO_MUTE_ON);
    }
}