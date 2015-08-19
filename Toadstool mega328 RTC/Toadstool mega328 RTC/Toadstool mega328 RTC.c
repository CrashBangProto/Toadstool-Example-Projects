/*
 *  @file Toadstool_mega328_RTC.c
 *
 *  Sample Program for Toadstool RTC-MCP 
 *  ====================================
 *
 *  The RTC-MCP is a real time clock module containing the Microchip MCP79400
 *
 *  Mount the Toadstool RTC module onto the Toadstool Mega328 board
 *
 *  This application performs an initialisation:
 *    - Initialises the RTC and starts the Oscillator
 *    - Checks whether the time on the RTC is set
 *    - If time is not set, it sets it to 31/12/2015 23:59:15
 *
 *  In the main loop the application reads the time every 5 seconds, and prints
 *  it out to the UART
 *
 *  ------------------------------------
 *  @author	Andrew Retallack, Crash-Bang Prototyping
 *			www.crash-bang.com
 *  @date	15/08/2015
 *
 *
 */

 //
 //**IMPORTANT**
 //Ensure that following is set:
 // F_CPU: CPU Speed used for delays, comms, etc.  eg. "F_CPU=16000000UL"
 //
 // SET IN: toolchain, avr/gnu C Compiler, Symbols
 //


/**********************************
*  Include Files
***********************************/
#include <avr/io.h>
#include "RTC_MCP79400.h"
#include "uart.h"

/**********************************
*  User-Defined Macros
***********************************/
#define RTC_ADDRESS 0b11011110	//I2C Address for the MCP79400 RTC.  Only first 7 bits form address.


/**********************************
*  Global Variables (for simplicity)
***********************************/
uint8_t tempVar = 0;	//Temporary Variable
uint8_t timeSec = 0;	//Time - Seconds
uint8_t timeMin = 0;	//Time - Minutes
uint8_t timeHr = 0;		//Time - Hours (24-hr)
uint8_t timeDay = 0;		//Date - Day
uint8_t timeWeekDay = 0;	//Date - Day of Week
uint8_t timeMonth = 0;		//Date - Month
uint16_t timeYear = 0;		//Date - Year
uint8_t timeAmPm = 0;		//Date - AM/PM indicator


int main(void)
{
	
	//Initialise the UART
	UART_Init(9600);
	UART_writeString("Welcome\r\n");
	
	//Initialise the I2C Interface at 200kHz
	I2C_init(200);
	
	//Initialise the RTC, and show whether oscillator started successfully
	if (RTC_Init(RTC_ADDRESS, 1, 1))
	{
		UART_writeString("Oscillator is Running\r\n");
	}
	else
	{
		UART_writeString("ERROR: Oscillator did NOT start\r\n");
	}
	
	
	//Read the Time from the RTC
	tempVar = RTC_GetTime(RTC_ADDRESS, &timeYear, &timeMonth, &timeDay, &timeWeekDay, &timeHr, &timeAmPm, &timeMin, &timeSec);	
	
	//If Year is one and month is one and day is one, assume time not set (these are the Power-On-Reset values).
	//Set the Time
	if ((timeYear == 1) && (timeMonth ==1) && (timeDay == 1))
	{
		tempVar = RTC_SetTime(RTC_ADDRESS, 15,12,31,5,23,1,59,15);
	}
	

	//Check if Oscillator Running after time was set
	tempVar = RTC_read(RTC_ADDRESS, MCP794_RTCWKDAY);	

	if (!(tempVar & (1<<MCP794_OSCRUN)))
	{
		UART_writeString("Oscillator NOT Running\r\n\r\n");
	}
	else
	{
		#if DEBUGLEVEL > 2
		UART_writeString("Oscillator is Running\r\n");
		#endif
	}
	
    while(1)
    {
		_delay_ms(5000);	//Only read the time every 5 seconds
		
		//Read the Time
		tempVar = RTC_GetTime(RTC_ADDRESS, &timeYear, &timeMonth, &timeDay, &timeWeekDay, &timeHr, &timeAmPm, &timeMin, &timeSec);	//Reset minutes to zero
		
		//Print the time over the UART
		UART_writeString("\r\nTimecheck: ");
		UART_printDecimal(timeDay,2);
		UART_writeString("/");
		UART_printDecimal(timeMonth,2);
		UART_writeString("/20");
		UART_printDecimal(timeYear,2);
		
		UART_writeString("   ");
		UART_printDecimal(timeHr,2);
		UART_writeString(":");
		UART_printDecimal(timeMin,2);
		UART_writeString(":");
		UART_printDecimal(timeSec,2);
		UART_writeString("\r\n");

		
    }
}




