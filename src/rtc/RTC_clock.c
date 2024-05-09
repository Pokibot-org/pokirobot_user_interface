#include "RTC_clock.h"

#include "stm32f4xx_hal.h"

#include <stdio.h>



// TODO
// Wednesday mys dudes

//==============================//
// Configuration
//==============================//

#define PCF8563_I2C_ADDR (0xA2>>0)

//==============================//
// Global variables
//==============================//

extern I2C_HandleTypeDef hi2c1;

RTC_time rtc_time;
int RTC_dma_request;

//==============================//
// Private I2C functions
//==============================//

void RTC_write_reg(int addr, uint8_t value) {
    HAL_I2C_Mem_Write(&hi2c1, PCF8563_I2C_ADDR, addr, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
}
uint8_t RTC_read(int addr) {
    uint8_t value;
    HAL_I2C_Mem_Read(&hi2c1, PCF8563_I2C_ADDR, addr, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
    return value;
}
void RTC_write(int addr, uint8_t* buffer, int length) {
    HAL_I2C_Mem_Write(&hi2c1, PCF8563_I2C_ADDR, addr, I2C_MEMADD_SIZE_8BIT, buffer, length, 100);
}
void RTC_read_dma(int addr, uint8_t* buffer, int length) {
    HAL_I2C_Mem_Read_DMA(&hi2c1, PCF8563_I2C_ADDR, addr, I2C_MEMADD_SIZE_8BIT, buffer, length);
    RTC_dma_request = 1;
}

//==============================//
// Public functions
//==============================//

void RTC_init() {
    // Disable TESTC
    RTC_write_reg(0x00, 0x00);
    RTC_dma_request = 0;
}

void RTC_update() {
    RTC_read_dma(0x02, (uint8_t*)(&rtc_time), 7);
}


// Weekday : 0 for monday, 1 for tuesday, and etc...
// up to 6 for sunday

void RTC_set(RTC_time newTime) {
    newTime.seconds = ((newTime.seconds/10)<<4) | (newTime.seconds%10);
    newTime.minutes = ((newTime.minutes/10)<<4) | (newTime.minutes%10);
    newTime.hours   = ((newTime.hours/10)  <<4) | (newTime.hours  %10);
    newTime.days    = ((newTime.days/10)   <<4) | (newTime.days   %10);
    newTime.months  = ((newTime.months/10) <<4) | (newTime.months %10);
    newTime.years   = ((newTime.years/10)  <<4) | (newTime.years  %10);
    RTC_write(0x02, (uint8_t*)(&newTime), 7);
}

void RTC_dma_callback() {
    RTC_dma_request = 0;
    // PCF8563 is giving data in BCD format.
    // Thus, converting here BCD values to integers.
    rtc_time.seconds = ((rtc_time.seconds>>4)&0x07)*10 + (rtc_time.seconds&0x0F);
    rtc_time.minutes = ((rtc_time.minutes>>4)&0x07)*10 + (rtc_time.minutes&0x0F);
    rtc_time.hours   = ((rtc_time.hours  >>4)&0x03)*10 + (rtc_time.hours  &0x0F);
    rtc_time.days    = ((rtc_time.days   >>4)&0x03)*10 + (rtc_time.days   &0x0F);
    rtc_time.months  = ((rtc_time.months >>4)&0x01)*10 + (rtc_time.months &0x0F);
    rtc_time.years   = ((rtc_time.years  >>4)&0x0F)*10 + (rtc_time.years  &0x0F);
}
