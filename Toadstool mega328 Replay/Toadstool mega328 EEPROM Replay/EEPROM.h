/*
 * @file	EEPROM.h
 *
 *  ------------------------------------
 *  @author	Andrew Retallack, Crash-Bang Prototyping
 *			www.crash-bang.com
 *  @date	15/02/2015
 *
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>
#include <util/delay.h>
#include "I2C.h"

#define EEPROM_ADDRESS_LOCATION	1	//Location in EEPROM to store last address location


uint8_t EEPROM_write(uint16_t deviceAddress, uint16_t memoryAddress, uint8_t data);
uint8_t EEPROM_read(uint16_t deviceAddress, uint16_t memoryAddress);
uint16_t EEPROM_getLastAddress(uint16_t deviceAddress);
void EEPROM_setLastAddress(uint16_t deviceAddress, uint16_t lastAddress);



#endif /* EEPROM_H_ */