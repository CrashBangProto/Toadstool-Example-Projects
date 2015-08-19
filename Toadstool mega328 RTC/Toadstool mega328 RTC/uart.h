/*
 * @file	uart.h
 *
 *  ------------------------------------
 *  @author	Andrew Retallack, Crash-Bang Prototyping
 *			www.crash-bang.com
 *  @date	15/02/2015
 *
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>



//Function Prototypes
void UART_Init(uint16_t baudRate);
void UART_writeChar(unsigned char data);
void UART_writeString(const char dataString[]);
void UART_printDecimal(uint16_t what, uint8_t padDigits);




#endif /* UART_H_ */