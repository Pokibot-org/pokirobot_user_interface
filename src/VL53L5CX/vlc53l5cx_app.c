#include "vlc53l5cx_app.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uint8_t status;
VL53L5CX_Configuration Dev;
VL53L5CX_ResultsData TOF_results;

int TOF_diffs[] = {0, 0, 0, 0};

int TOF_init() {
    uint8_t isAlive;

    Dev.platform.address = VL53L5CX_DEFAULT_I2C_ADDRESS;
    status = vl53l5cx_is_alive(&Dev, &isAlive);
    if (!isAlive || status) {
        return status;
    }
    status = vl53l5cx_init(&Dev);
    if (status) {
        return status;
    }

    status = vl53l5cx_set_ranging_frequency_hz(&Dev, 60);
    if (status) {
        return status;
    }
    status = vl53l5cx_set_target_order(&Dev, VL53L5CX_TARGET_ORDER_CLOSEST);

    status = vl53l5cx_start_ranging(&Dev);
    if (status) {
        return status;
    }
    return VL53L5CX_STATUS_OK;
}

int inMovement = 0;
int movementDx = 0;
int movementDy = 0;

int TOF_updateData() {
    uint8_t isReady;
    status = vl53l5cx_check_data_ready(&Dev, &isReady);
    if (isReady) {
        status = vl53l5cx_get_ranging_data(&Dev, &TOF_results);
        if (status) {
            return 0;
        }

        // int integration_time_ms;
        // /* Get current integration time */
        // status = vl53l5cx_get_integration_time_ms(&Dev, &integration_time_ms);
        // if (status) {
        //     return 0;
        // }
        // printf("%d ms\r\n", integration_time_ms);


        int minSample = TOF_results.distance_mm[0];
        int maxSample = TOF_results.distance_mm[0];
        for (int i = 0; i < 16; i++) {
            if (TOF_results.distance_mm[i] > 256) {
                TOF_results.distance_mm[i] = 0;
            }
            if (TOF_results.distance_mm[i] < minSample) {
                minSample = TOF_results.distance_mm[i];
            }
            if (TOF_results.distance_mm[i] > maxSample) {
                maxSample = TOF_results.distance_mm[i];
            }
        }

        int bestValue = 0;
        int bestPosition = 0;
        for (int i = 0; i < 16; i++) {
            int value = 0;
            int coeff;
            int coeffSum = 0;
            for (int j = 0; j < 16; j++) {
                coeff = 7 - (abs((i&0x03) - (j&0x03)) + abs((i>>2) - (j>>2)));
                int sample = TOF_results.distance_mm[j];

                value+=sample*coeff;
                coeffSum+=coeff;
            }
            value = value / coeffSum;
            if (value > bestValue) {
                bestValue = value;
                bestPosition = i;
            }
        }
        int resultX = 3-(bestPosition&0x03);
        int resultY = bestPosition>>2;
        if (inMovement) {
            if (bestValue < 10) {
                inMovement = 0;
                if (abs(movementDx) > abs(movementDy) && abs(movementDx) > 2) {
                    if (movementDx > 0) {
                        return SCROLL_RIGHT;
                    } else {
                        return SCROLL_LEFT;
                    }
                } else if (abs(movementDy) > abs(movementDx) && abs(movementDy) > 2) {
                    if (movementDy > 0) {
                        return SCROLL_DOWN;
                    } else {
                        return SCROLL_UP;
                    }
                }
            } else {
                int newX = (2*TOF_diffs[0] + 16*resultX) / 3;
                int newY = (2*TOF_diffs[1] + 16*resultY) / 3;
                TOF_diffs[2] = bestValue;
                movementDx = movementDx + (newX-TOF_diffs[0]);
                movementDy = movementDy + (newY-TOF_diffs[1]);
                TOF_diffs[0] = newX;
                TOF_diffs[1] = newY;
            }
        } else if (bestValue > 20) {
            inMovement = 1;
            TOF_diffs[0] = (16*resultX);
            TOF_diffs[1] = (16*resultY);
            TOF_diffs[2] = bestValue;
            movementDx = 0;
            movementDy = 0;
        }
    }
    return TOF_NO_EVENT;
}







      // TOF_event = TOF_updateData();
      // for (int i = 0; i < 16; i++) {
      //   int rb = TOF_results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i];
      //   if (rb > 255) {
      //     rb = 0xFFFF0000;
      //   } else {
      //     rb = (0xFF<<24) | (rb<<16) | (rb<<8) | (rb);
      //   }
      //   BSP_LCD_SetTextColor(rb);
      //   BSP_LCD_FillRect((3-(i&0x03))*32, (i>>2)*32, 32, 32);
      // }
      // int tc = TOF_diffs[2];
      // if (TOF_diffs[2] > 20) {
      //   BSP_LCD_SetTextColor(0xFFFFFF00);
      //   BSP_LCD_FillRect(TOF_diffs[0]*2+16, TOF_diffs[1]*2+16, 2, 2);
      // }
