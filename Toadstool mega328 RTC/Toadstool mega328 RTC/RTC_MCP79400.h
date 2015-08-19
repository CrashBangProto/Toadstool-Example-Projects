/*
 * RTC_MCP79400.h
 *
 * Created: 30/05/2015 15:45:10
 *  Author: Andrew
 */ 


#ifndef RTC_MCP79400_H_
#define RTC_MCP79400_H_

#include <avr/io.h>
#include <util/delay.h>
#include "I2C.h"



#define MCP794_RTCSEC 0x00
#define MCP794_ST 7
#define MCP794_SECTEN2 6
#define MCP794_SECTEN1 5
#define MCP794_SECTEN0 4
#define MCP794_SECONE3 3
#define MCP794_SECONE2 2
#define MCP794_SECONE1 1
#define MCP794_SECONE0 0
#define MCP794_MASK_Second 0b01111111

#define MCP794_RTCMIN (0x01)
#define MCP794_MINTEN2 6
#define MCP794_MINTEN1 5
#define MCP794_MINTEN0 4
#define MCP794_MINONE3 3
#define MCP794_MINONE2 2
#define MCP794_MINONE1 1
#define MCP794_MINONE0 0

#define MCP794_RTCHOUR (0x02)
#define MCP794_12_24 6
#define MCP794_AM_PM 5
#define MCP794_HRTEN1 5
#define MCP794_HRTEN0 4
#define MCP794_HRONE3 3
#define MCP794_HRONE2 2
#define MCP794_HRONE1 1
#define MCP794_HRONE0 0
#define MCP794_MASK_24Hour 0b00111111
#define MCP794_MASK_12Hour 0b00011111

#define MCP794_RTCWKDAY 0x03
#define MCP794_OSCRUN 5
#define MCP794_PWRFAIL 4
#define MCP794_VBATEN 3
#define MCP794_WKDAY2 2
#define MCP794_WKDAY1 1
#define MCP794_WKDAY0 0

#define MCP794_RTCDATE (0x04)
#define MCP794_DATETEN1 5
#define MCP794_DATETEN0 4
#define MCP794_DATEONE3 3
#define MCP794_DATEONE2 2
#define MCP794_DATEONE1 1
#define MCP794_DATEONE0 0

#define MCP794_RTCMTH 0x05
#define MCP794_LPYR 5
#define MCP794_MTHTEN0 4
#define MCP794_MTHONE3 3
#define MCP794_MTHONE2 2
#define MCP794_MTHONE1 1
#define MCP794_MTHONE0 0
#define MCP794_MASK_Month 0b00011111

#define MCP794_RTCYEAR 0x06
#define MCP794_YRTEN3 7
#define MCP794_YRTEN2 6
#define MCP794_YRTEN1 5
#define MCP794_YRTEN0 4
#define MCP794_YRONE3 3
#define MCP794_YRONE2 2
#define MCP794_YRONE1 1
#define MCP794_YRONE0 0

#define MCP794_CONTROL 0x07
#define MCP794_OUT 7
#define MCP794_SQWEN 6
#define MCP794_ALM1EN 5
#define MCP794_ALM0EN 4
#define MCP794_EXTOSC 3
#define MCP794_CRSTRIM 2
#define MCP794_SQWFS1 1
#define MCP794_SQWFS0 0



//Function Prototypes
uint8_t RTC_Init(uint8_t deviceAddress, uint8_t is24Hour, uint8_t isBackupBat);
uint8_t RTC_SetTime(uint8_t deviceAddress, uint16_t setYear, uint8_t setMonth, uint8_t setDay, uint8_t setWeekDay, uint8_t setHour, uint8_t isHourPM, uint8_t setMinutes, uint8_t setSeconds);
uint8_t RTC_GetTime(uint8_t deviceAddress, uint16_t *getYear, uint8_t *getMonth, uint8_t *getDay, uint8_t *getWeekDay, uint8_t *getHour, uint8_t *isHourPM, uint8_t *getMinutes, uint8_t *getSeconds);
uint8_t RTC_write(uint8_t deviceAddress, uint8_t registerAddress, uint8_t data);
uint8_t RTC_read(uint8_t deviceAddress, uint8_t registerAddress);
uint8_t decToBcd(uint8_t val);
uint8_t bcdToDec(uint8_t val);


#endif /* RTC_MCP79400_H_ */