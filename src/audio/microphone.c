#include "fatfs.h"
#include "stm32469i_discovery.h"
#include <stdio.h>

FIL fil;
FSIZE_t fsize;
UINT read_bytes;

int samplesToRecord;

extern I2S_HandleTypeDef hi2s3;

#define PACKET_SIZE 2048
uint8_t* recordBuffer;
int* recordBufferWp;
int recordBufferSize;


int MIC_record(uint8_t* buffer, int bufferSize, int* bufferWp, int samples) {
    samplesToRecord = samples;
    recordBufferWp = bufferWp;
    recordBuffer = buffer;
    recordBufferSize = bufferSize;

    printf("Start\r\n");

    // HAL_I2S_Receive_DMA(&hi2s3, (uint16_t*)(recordBuffer+(uint8_t*)*recordBufferWp), PACKET_SIZE/2);
    samplesToRecord = samplesToRecord - PACKET_SIZE;

    printf("Recording...\r\n");

    return 0;
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef * hi2s) {
    // printf("I2S it\r\n");

    if (*recordBufferWp + PACKET_SIZE >= recordBufferSize) {
        *recordBufferWp = 0;
    } else {
        *recordBufferWp = *recordBufferWp + PACKET_SIZE;
    }

    if (samplesToRecord - PACKET_SIZE >= 0) {
        // HAL_I2S_Receive_DMA(&hi2s3, (uint16_t*)(recordBuffer+(uint8_t*)*recordBufferWp), PACKET_SIZE/2);
        samplesToRecord = samplesToRecord - PACKET_SIZE;
    } else {
        printf("Stop\r\n");
    }
}


// Test dans main ci dessous


  // if (f_open(&fil, "MIC.bin", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
  //   // UINT read_bytes;
  //   // f_write(&fil, (uint8_t *)Buffers[0], 8, &read_bytes);
  //   f_close(&fil);
  //   printf("File created\r\n");
  // }
  // uint8_t recordBuffer[16384];
  // int recordBufferWp = 0;
  // int recordBufferRp = 0;
  // MIC_record(recordBuffer, 16384, &recordBufferWp, 262144);
  // uint32_t tim4_d;




      // int nextRp = (recordBufferRp + 1024 >= 16384) ? 0 : recordBufferRp + 1024;
      // int distance = recordBufferRp <= recordBufferWp ? recordBufferWp - recordBufferRp : recordBufferWp + 16384 - recordBufferRp;
      // if (distance >= 1024) {
      //   if (f_open(&fil, "MIC.bin", FA_OPEN_APPEND | FA_WRITE) == FR_OK) {
      //     UINT read_bytes;
      //     while (distance >= 1024) {
      //       f_write(&fil, (uint8_t *)(recordBuffer+recordBufferRp), 1024, &read_bytes);
      //       recordBufferRp = nextRp;
      //       // printf("Written %04X %04X %04X\r\n", read_bytes, recordBufferRp, recordBufferWp);
      //       nextRp = (recordBufferRp + 1024 >= 16384) ? 0 : recordBufferRp + 1024;
      //       distance = recordBufferRp <= recordBufferWp ? recordBufferWp - recordBufferRp : recordBufferWp + 16384 - recordBufferRp;
      //     }
      //     f_close(&fil);
      //   } else {
      //     printf("Ferror\r\n");
      //   }
      // }