/*
 * @file	uart.c
 *
 *  ------------------------------------
 *  @author	Andrew Retallack, Crash-Bang Prototyping
 *			www.crash-bang.com
 *  @date	30/05/2015
 *
 */ 

#include "uart.h"


/**
* @brief	Initialise the UART in Asynchronous Mode
*
* @details	This routine initialises the UART in Asynchronous mode by:
*				1. setting baud rate
*				2. setting data frame (8 bits, no parity, 1 stop bit)
*				3. enables the transmitter and received
*
*
* @param[in]	baudRate	The baud rate to operate at
*
* @return	none
************************************************************************/
void UART_Init(uint16_t baudRate)
{
	uint16_t my_UBRR;

	//Calculate the value of the UBRR register, to set the Baud Rate
	my_UBRR = (F_CPU/16UL/baudRate) - 1;

	//Set the Baud ratee
	UBRR0H = (unsigned char)(my_UBRR >> 8);		//Set the High register
	UBRR0L = (unsigned char)my_UBRR;			//Set the Low register

	//Stop Bit: 1 - ensure bit is unset (incase it was set before)
	UCSR0C &= ~(1<<USBS0);

	//Data Bits: 8
	UCSR0C |= ( (1<<UCSZ00) | (1<<UCSZ01) );

	//Parity: None - ensure bits unset (incase they were set before)
	UCSR0C &= ~( (1<<UPM00) | (1<<UPM01) );

	//Mode: Ensure Asynchronous Mode (clear bits incase they were set before)
	UCSR0C &= ~( (1<<UMSEL00) | (1<<UMSEL01) );
	
	//Enable the Transmitter and Receiver
	UCSR0B |= (1<<TXEN0) | (1<<RXEN0);

}


/**
* @brief	Writes a single character to the UART
*
* @details	This routine writes a single character to the UART
*
* @param[in]	data	The character to write
*
* @return	none
************************************************************************/
void UART_writeChar(unsigned char data)
{
	
	//Wait until transmit buffer empty
	while ( !(UCSR0A & (1<<UDRE0) ) );
	
	//write char
	UDR0 = data;
	
}


/**
* @brief	Writes a string of characters to the UART
*
* @details	This routine writes a string of characters to the UART
*
* @param[in]	dataString	The string to write
*
* @return	none
************************************************************************/
void UART_writeString(const char dataString[])
{
	uint16_t i = 0;
	
	//Loop through the string, writing character by character
	while(dataString[i])
	{
		UART_writeChar(dataString[i]);	//write each string
		i++;
	}
}


/**
* @brief	Writes numeric value to UART in digits
*
* @details	This routine writes a numeric value out in digits.  The value needs to
*			be converted to a string of ASCII characters before being written.
*			May be optionally padded to specified length
*
* @param[in]	numericVal	The value to write
* @param[in]	padDigits	Left-Pad with zeros? Specifies final length of string. 0 = none
* @return	none
************************************************************************/
void UART_printDecimal(uint16_t numericVal, uint8_t padDigits)
{
	
	char numberText[6]="";
	uint16_t workingNumber = numericVal;
	char tempWorkNum;
	uint16_t workingDivisor = 10000;
	uint8_t digitNumber = 0;
	
	//Generate the individual digits by successively dividing by multiples of 10, and storing in a string variable
	for (char i = 4; i>0; i--)
	{
		tempWorkNum = (workingNumber / (uint16_t)( workingDivisor ));
		
		//If the tempWorkNum is zero, then only record the digit if this is not the first digit (i.e. discard leading zeros)
		//If the tempWorkNum is non-zero, then record the digit
		if ( ((tempWorkNum == 0) && digitNumber > 0) || (tempWorkNum > 0) || (i < padDigits))
		{
			numberText[digitNumber] = tempWorkNum + 48;
			digitNumber++;
			
		}
		
		//Move one more digit to the right
		workingNumber = workingNumber % workingDivisor;	//Remove the digit already processed
		workingDivisor /= 10;
	}
	
	//The last digit in the number
	numberText[digitNumber] = (workingNumber % 10) + 48;
	
	//Finally, write the number out as text.
	UART_writeString(numberText);

}
