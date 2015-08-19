/**
 * @file I2C.c
 * A Simple driver to handle TWI(I2C)-Related functionality
 *
 *  ------------------------------------
 *  @author	Andrew Retallack, Crash-Bang Prototyping
 *			www.crash-bang.com
 *  @date	15/02/2015
 */ 


#include "I2C.h"

/**
* @brief	Initialise the TWI
*
* @details	This routine initialises the TWI by: calculating Bit-rate, setting pre-scaler & bit-rate
*
* @param[in]	I2C_kHz	The I2C bus speed in kHz (eg. 100 = 100kHz)
* 
* @return	none
************************************************************************/
void I2C_init(uint8_t I2C_kHz)
{
	uint8_t bitRate;
	volatile uint16_t I2C_Hz = (unsigned long)I2C_kHz * 1000UL;
	
	bitRate = (F_CPU / (I2C_Hz * 2 * I2C_PRESCALER)) - (16 / (2 * I2C_PRESCALER) );	//Calculate bit-rate
	
	TWSR &= ~((1<<TWPS0)|(1<<TWPS1));		//Clear the pre-scaler
	TWSR |= I2C_PRESCALER_BIT;				//Set the new pre-scaler
	
	TWBR = bitRate;	//Set the bit rate
	
}



/**
* @brief	Send a TWI Start Condition
*
* @details	This function enables the TWI and sends a Start Condition
*
* @return	the result of the operation - the TWI Status bits from TWI Status Register
************************************************************************/
uint8_t I2C_sendStart(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //Clear the Interrupt Flag, Set Start bit, Enable TWI
	
	I2C_waitComplete();	//Wait for transmission to complete
		
	return TW_STATUS;	//Success
	
}



/**
* @brief	Send a TWI Stop Condition
*
* @details	This function sends a Stop Condition and clears the TWI Interrupt Flag
*
* @return	none
************************************************************************/
void I2C_sendStop(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN); //Clear the Interrupt Flag, Set Stop bit, Enable TWI
}



/**
* @brief	Send a byte over the TWI interface
*
* @details	This function sends a single byte over the TWI interface
*
* @param[in]	Data The byte to be sent
*
* @return	the result of the operation - the TWI Status bits from TWI Status Register
************************************************************************/
uint8_t I2C_send(uint8_t Data)
{
	TWDR = Data;	//Set the Data to send
	TWCR = (1<<TWINT) | (1<<TWEN);		//Clear interrupt flag and enable TWI
	
	I2C_waitComplete();	//Wait for transmit to complete
	
	return TW_STATUS;	//Return the status
	
}



/**
* @brief	Receive a byte over the TWI interface
*
* @details	This function receives a single byte over the TWI interface and sends an ACK or NACK
*
* @param[in]	sendAck 1=Send an ACK; any other = do not send ACK
*
* @return	The data read from TWI (0 if an error occurred)
************************************************************************/
uint8_t I2C_read(uint8_t sendAck)
{
	uint8_t	ackBit = 0;
	uint8_t statusCheck = TW_MR_DATA_NACK;	//Assume status check has no ACK
	
	if (sendAck == 1)
	{
		ackBit = (1<<TWEA);	//Send ACK
		statusCheck = TW_MR_DATA_ACK;	//status check has ACK
	}


	TWCR = (1<<TWINT) | (1<<TWEN) | ackBit;				//Clear Interrupt, Enable TWI, send ACK if bit is set
	
	I2C_waitComplete();	//Wait for Transmission to complete
	
	//Check Status
	
	if (TW_STATUS != statusCheck)
	{
		return 0;	//An Error
	}
	
	//Otherwise return data
	return TWDR;
	
}



/**
* @brief	Wait for Transmission to Complete
*
* @details	This function waits for the TWI transmission to complete (using TWI Interrupt)
*
* @return	the result of the operation - the TWI Status bits from TWI Status Register
************************************************************************/
void I2C_waitComplete(void)
{
	
	while (!(TWCR & (1<<TWINT)) );	//Wait for the TWINT interrupt flag to be set - indicates transmission complete
	
}

