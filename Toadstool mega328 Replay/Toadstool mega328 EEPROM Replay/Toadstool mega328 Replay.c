/*
 * @file	Toadstool mega328 EEPROM Replay.c
 *
 *  Sample Program for Toadstool 24LC EEPROM
 *  ========================================
 *  Mount the Toadstool 24LC EEPROM module onto the Toadstool Mega328 board
 *
 *  This application replays a sequence of LED flashes, at a sample rate of 100ms.
 *  The sequence is stored on the EEPROM, and can be programmed by the user.
 *  
 *  1. To re-initialise the EEPROM to an 800ms on/off sequence, hold switch down
 *		while powering on the Toadstool.  
 *		3 flashes = initialisation starting; 5 flashes = initialisation complete.
 *
 *  2. To record a sequence, press the switch anytime while replaying.
 *		3 flashes indicate recording has begun
 *		Use the switch to record a sequence
 *		5 flashes indicate the recording is complete
 *
 *	Replay mode starts:
 *	- after power-on initialisation is complete (LED is lit for 3 seconds)
 *	- after EEPROM re-initialisation (1 above) is complete
 *	- after recording is complete
 *    
 *  ------------------------------------
 *  @author	Andrew Retallack, Crash-Bang Prototyping 
 *			www.crash-bang.com
 *  @date	15/02/2015
 *
 *
 *  CONNECTIONS
 *  ===========
 *  Connect pushbutton between PB0 and GND
 *  Connect LED to PB1 (anode)
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
*  User-Defined Macros
***********************************/
#define EEPROM_DEVICE_ADDRESS 0b10100110	//EEPROM's Device Address on the I2C bus.  Only 7 MSB bits used, LSB bit is always zero
#define EEPROM_FIRST_ADDRESS 20		//First address for storing recorded data
#define EEPROM_MAX_ADDRESS 15999	//Maximum address that can be written to the EEPROM (128kBit: 128,000 / 8)

#define REPLAY_SECS			5		//Number of seconds to record and replay
#define REPLAY_SAMPLE_MS	100		//Number of milliseconds per sample
#define REPLAY_COUNT		(uint16_t)(REPLAY_SECS * 1000 / REPLAY_SAMPLE_MS / 8)	//Number of samples being recorded/replayed

#define PIN_LED PB1			//Connect LED to PB1
#define PIN_SWITCH PB0		//Connect SWITCH to PB0

//Define the possible states
#define STATE_REPLAY	0
#define STATE_START_REC	1
#define STATE_RECORDING	2
#define STATE_STOP_REC	3


/**********************************
*  Include Files
***********************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "I2C.h"			//Simple library of I2C (TWI) routines
#include "EEPROM.h"			//Simple library of EEPROM routines


/**********************************
*  Function Prototypes
***********************************/
void configPins(void);
void configTimer(void);
void flashLED(void);
void clearMemory(void);
void replay1Bit(void);
void record1Bit(void);


/**********************************
*  Global Variables (for simplicity)
***********************************/
volatile uint8_t currentState;	//Current State: replay / start record / recording / end record
volatile uint16_t currentMemLocation;	//In replay/record state, the current EEPROM memory location
volatile uint8_t currentMemBit;			//In replay/record state, the bit of the current EEPROM memory location
volatile uint8_t isrFlag;	//Flag Interrupt
volatile uint8_t LEDValue;	//Byte containing 8 sequential LED values (1 per bit)



int main(void)
{
    
	configPins();	//Configure the pins for input and output
	
	//Turn LED on for 3 secs to indicate initialisation, and allow time to press switch (and enter Memory Init)
	PORTB |= (1<<PIN_LED);
	_delay_ms(3000);
	PORTB &= ~(1<<PIN_LED);

	configTimer();	//Configure Timer to fire every 100ms
	
	I2C_init(100);	//Initialise TWI(I2C) communication at 100kHz
	

	currentState = STATE_REPLAY;	//Start in the Replay State
	
	currentMemLocation = EEPROM_FIRST_ADDRESS;	//Start replaying from the first memory location
	currentMemBit = 8;	//This forces a read as it is outside the allowed range of 0-7
	
		
	//Check whether button is pressed - if pressed, then pin will be LOW
	//Holding button down during reset will re-initialise the EEPROM memory
	if ((PINB & (1<<PIN_SWITCH)) == 0)
	{
		clearMemory();
	}
	
	sei();	//Enable Interrupts so Timer interrupts fire
	
	
	while(1)
    {
        
		//Has the timer interrupt fired?  If so, process
		if (isrFlag == 1)
		{
			
			isrFlag = 0;	//Reset ISR flag to off
			
			//Decide what to do based on current state
			switch (currentState)
			{
				
				//STATE: Replaying the recording
				case STATE_REPLAY:
				
					//First check whether button pressed.  If so, need to enter recording state
					if( (PINB & (1<<PIN_SWITCH)) == 0)
					{
						currentState = STATE_START_REC;
					}
					
					//Otherwise, replay another bit from the recording (1 bit per 100ms)
					else
					{
						replay1Bit();	
						break;
					}
				
				
				//STATE: Start the recording
				case STATE_START_REC:	
					//Flash the LED 3 times so we know recording is about to start
					
					TIMSK1 &= ~(1<<OCIE1A);	//Disable interrupts on Timer
					
					flashLED();
					flashLED();
					flashLED();
					
					TIMSK1 |= (1<<OCIE1A);	//Enable interrupts on Timer
					
					//Reset memory location and bit to first address
					currentMemLocation = EEPROM_FIRST_ADDRESS;
					currentMemBit = 0;
					
					
					//Set state to recording
					currentState = STATE_RECORDING;
					
					break;
					
				
				//STATE: Continue recording
				case STATE_RECORDING:	
					//We'll now record and store the value of the button (and therefore LED) every 100ms
					record1Bit();
					break;
					
					
				//STATE: Stop the recording
				case STATE_STOP_REC:	
					//Flash the LED 5 times so we know recording is finished
					
					TIMSK1 &= ~(1<<OCIE1A);	//Disable interrupts on Timer
					
					flashLED();
					flashLED();
					flashLED();
					flashLED();
					flashLED();
					
					TIMSK1 |= (1<<OCIE1A);	//Enable interrupts on Timer
					
					//Reset memory location
					currentMemLocation = EEPROM_FIRST_ADDRESS;
					currentMemBit = 0;
					
					currentState = STATE_REPLAY;	//Enter Replay mode
				
			
			}
		
		}
		

    }
	



}



/**
* @brief	Configure IO pins
*
* @details	Configure PIN_LED as output, 
*			and PIN_SWITCH as input with internal pull-up resistors
*
* @return	none
************************************************************************/
void configPins(void)
{
		//Configure the pin the LED is connected to
		DDRB |= (1<<PIN_LED);	//Set pin as an output
		
		//Configure the pin the SWITCH is connected to
		DDRB &= ~(1<<PIN_SWITCH);	//Set pin as an input
		PORTB |= (1<<PIN_SWITCH);	//Enable pull-up resistor on pin

}



/**
* @brief	Flash LED
*
* @details	This function flashes the LED on and off for 150ms
*
* @return	none
************************************************************************/
void flashLED(void)

{

	PORTB |= (1<<PIN_LED);		//Turn the LED on, by making pin go high
	_delay_ms(150);
	PORTB &= ~(1<<PIN_LED);		//Turn the LED off, by making pin go low
	_delay_ms(150);
}



/**
* @brief	Initialise the Timer
*
* @details	This function initialises Timer1 to trigger an interrupt when Compare occurs.
*			Timer Interval = 100 msecs
*
* @return	none
************************************************************************/
void configTimer(void)
{

	
	//Order of setting registers as per data sheet
	
	TIMSK1 &= ~( (1<<OCIE1A) | (1<<OCIE1B) | (1<<TOIE1) );	//Disable interrupts on timer
	
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10); //Set to CTC (compare) mode, set prescaler to 1024
	
	TCCR1A = (0<<WGM11)|(0<<WGM10);		//Set to CTC (compare) mode
	
	OCR1A = 1563;	// Crystal = 16MHz; Prescaler = 1024; cycles per sec = 15625; Interval = 100ms

	TIMSK1 |= (1<<OCIE1A);	//Enable interrupts on Compare
	
}



/**
* @brief	Re-initialise the EEPROM memory
*
* @details	Initialise the EEPROM memory to an 800ms on/off flash pattern
*
* @return	none
************************************************************************/
void clearMemory(void)
{
	
	uint8_t iCount;
	
	currentMemLocation = EEPROM_FIRST_ADDRESS;	//Start at the first memory address
	
	//Three flashes to show memory being set
	flashLED();
	flashLED();
	flashLED();
	
	//Leave the LED on during initialisation to show it is in progress
	PORTB |= (1<<PIN_LED);
	
	

	//Loop through the number of memory locations used
	//Why did we choose 800ms as an interval?  Because each bit represents 100ms, and 8 bits to a byte, so 8*100ms = 800ms
	for (iCount=0; iCount <= (REPLAY_COUNT/2); iCount++)
	{
		//For the first 800ms, set the EEPROM to 1 (LED is ON)
		EEPROM_write(EEPROM_DEVICE_ADDRESS, currentMemLocation, 0b11111111);
		currentMemLocation++;
		
		//For the second 800ms, set the EEPROM to 0 (LED is OFF)
		EEPROM_write(EEPROM_DEVICE_ADDRESS, currentMemLocation, 0b00000000);
		currentMemLocation++;
	}
	

	//Turn LED off
	_delay_ms(1000);	//delay for UI reasons only
	PORTB &= ~(1<<PIN_LED);
	
	
	//Five flashes to show finished
	flashLED();
	flashLED();
	flashLED();
	flashLED();
	flashLED();
	
	
	//Reset memory location 
	currentMemLocation = EEPROM_FIRST_ADDRESS;
	currentMemBit = 8;	//This forces a read as it is outside the allowed range of 0-7

}



/**
* @brief	Replay one bit from EEPROM
*
* @details	Replay the current bit from EEPROM, and read next byte from 
*			EEPROM if needed.
*
* @return	none
************************************************************************/
void replay1Bit(void)
{

	//Have we finished processing all bits for this Memory location's byte?
	if (currentMemBit > 7)	//Yes: So read the next byte
	{
							
		//Read the next byte from the current EEPROM memory location
		LEDValue = EEPROM_read(EEPROM_DEVICE_ADDRESS, currentMemLocation);
							
		//Increment to the next memory location
		currentMemLocation++;
							
		//If we've been going for more than the REPLAY Sample Count, then start from beginning
		if (currentMemLocation > (REPLAY_COUNT + EEPROM_FIRST_ADDRESS))
		currentMemLocation = EEPROM_FIRST_ADDRESS;
							
		//Start at bit 0 of current byte
		currentMemBit = 0;
							
	}
						
	//Set the LED to current bit
	if ( (LEDValue & (1<<currentMemBit)) == 0)	//If current bit is 0
	{
		PORTB &= ~(1<<PIN_LED);	//Turn LED off
	}
	else
	{
		PORTB |= (1<<PIN_LED);	//Turn LED on
	}
						
	//Move onto next bit, for next time the timer interrupt fires
	currentMemBit ++;
	
}



/**
* @brief	Record one bit
*
* @details	Record the current bit, and if the current byte is "full" 
*			then write it to EEPROM
*
* @return	none
************************************************************************/
void record1Bit(void)
{
	//If this is the first bit of this memory location, then then set entire byte to zero
	if (currentMemBit == 0)
		LEDValue = 0;
					
	//Is the switch pressed?
	if( (PINB & (1<<PIN_SWITCH)) == 0)	//YES:
	{
		LEDValue |= (1<<currentMemBit);	//Set the value of this bit to a "1" so show switch was on
		PORTB |= (1<<PIN_LED);			//Turn LED on to give feedback to user
	}
	else  //NO:
	{
		//No need to set the value of the bit as the byte starts out as zero
		PORTB &= ~(1<<PIN_LED);			//Turn LED off to give feedback to user
	}
					
	
	currentMemBit++;	//Move to next bit
					
	//Have we finished recording all bits for this Memory Location?
	if (currentMemBit > 7)	//Yes: So write the full byte
	{
						
		//Write the byte to the current EEPROM memory location
		EEPROM_write(EEPROM_DEVICE_ADDRESS, currentMemLocation, LEDValue);
						
		//Increment to the next memory location
		currentMemLocation++;
						
		//If we've been going for more than the REPLAY Sample Count, then end the recording
		if (currentMemLocation > (REPLAY_COUNT + EEPROM_FIRST_ADDRESS))
		{
			currentState = STATE_STOP_REC;	//Move onto the "Stop Recording" state
		}
		else
		{
			//Start again at bit 0 of byte
			currentMemBit = 0;
		}
						
	}
	
}



/**
* @brief	Interrupt Handler for Timer1A Compare
*
* @details	Not called from user code.  Called by Timer1 Compare interrupt.
*
* @return	none
************************************************************************/
ISR(TIMER1_COMPA_vect)
{

	//Set the flag to indicate Interrupt has fired
	isrFlag = 1;

}