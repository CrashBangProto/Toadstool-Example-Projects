/*
 * @file	I2C.h
 *
 *  ------------------------------------
 *  @author	Andrew Retallack, Crash-Bang Prototyping
 *			www.crash-bang.com
 *  @date	15/02/2015
 *
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include <util/twi.h>
#include "uart.h"


#ifndef F_CPU
#error "F_CPU not defined"
#endif

#if F_CPU > 4000000UL
#define I2C_PRESCALER 4					//Value of prescaler for calculation of bitrate
#define I2C_PRESCALER_BIT (1<<TWPS0)	//Prescaler Bits to be set in TWSR register
#else
#define I2C_PRESCALER 1					//Value of prescaler for calculation of bitrate
#define I2C_PRESCALER_BIT 0				//Prescaler Bits to be set in TWSR register
#endif


void I2C_init(uint8_t I2C_kHz);
uint8_t I2C_sendStart(void);
void I2C_sendStop(void);
uint8_t I2C_send(uint8_t Data);
uint8_t I2C_read(uint8_t sendAck);
void I2C_waitComplete(void);




#endif /* I2C_H_ */
