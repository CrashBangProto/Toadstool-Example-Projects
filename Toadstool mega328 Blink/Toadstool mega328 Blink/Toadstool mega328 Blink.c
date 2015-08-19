/*
 * Toadstool_mega328_Blink.c
 *
 *  Sample Program for Toadstool mega328
 *  ====================================
 *  Blinks an LED connected to PB0 every second.
 *  Connect components as set out in the Getting Started guide
 *  
 *  ------------------------------------
 *  Author: Andrew Retallack
 *  Crash-Bang Prototyping www.crash-bang.com
 *
 */ 

#define F_CPU 16000000UL	//We are running at 16MHz. Used to time the delay

#include <avr/io.h>			
#include <util/delay.h>		//Contains the _delay_ms function called later

int main(void)
{

	//Configure the port the LED is connected to (PB0)
	DDRB |= (1<<DDB0);	//Set PB0 as an output

    while(1)	//Loop indefinitely
    {
		//LED on
		PORTB |= (1<<PORTB0);		//Turn the LED on, by making PB0 go high
		_delay_ms(1000);			//Delay 1 second (1000 milliseconds)
		
		//LED off
		PORTB &= ~(1<<PORTB0);		//Turn the LED off, by making PB0 go low
		_delay_ms(1000);			//Delay 1 second (1000 milliseconds)
		
    }
	
}