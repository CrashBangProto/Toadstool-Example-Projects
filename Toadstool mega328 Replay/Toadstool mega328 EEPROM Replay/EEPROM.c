/*
 * @file	EEPROM.c
 *
 *  ------------------------------------
 *  @author	Andrew Retallack, Crash-Bang Prototyping
 *			www.crash-bang.com
 *  @date	15/02/2015
 *
 */ 

#include "EEPROM.h"

/**
* @brief	Retrieve address of last Logged item
*
* @details	This function helps to implement simple logging, 
*			by retrieving the EEPROM address of the last item logged
*
* @param[in]	deviceAddress	The I2C address of the EEPROM device
*
* @return	The last address in EEPROM
************************************************************************/
uint16_t EEPROM_getLastAddress(uint16_t deviceAddress)
{
	uint8_t lastHigh;
	uint8_t lastLow;
	
	//Addresses are 2 bytes, so need to read High and Low bytes
	lastHigh = EEPROM_read(deviceAddress,EEPROM_ADDRESS_LOCATION);
	lastLow = EEPROM_read(deviceAddress,EEPROM_ADDRESS_LOCATION + 1);
	
	return (uint16_t)(lastHigh<<8) | lastLow;

}



/**
* @brief	Store address of last Log
*
* @details	This function helps to implement simple logging,
*			by storing the EEPROM address of the last item logged
*
* @param[in]	deviceAddress	The I2C address of the EEPROM device
* @param[in]	lastAddress		The EEPROM memory location of last logged item
*
* @return	none
************************************************************************/
void EEPROM_setLastAddress(uint16_t deviceAddress, uint16_t lastAddress)
{
	uint8_t lastHigh;
	uint8_t lastLow;
	
	//Addresses are 2 bytes, so need to write High and Low bytes
	lastHigh = (lastAddress >> 8);
	lastLow = (uint8_t)lastAddress;
	
	EEPROM_write(deviceAddress, EEPROM_ADDRESS_LOCATION, lastHigh);
	EEPROM_write(deviceAddress, EEPROM_ADDRESS_LOCATION+1, lastLow);
	
}


/**
* @brief	Write data to the EEPROM
*
* @details	This function writes data to the EEPROM from a specified address
*
* @param[in]	deviceAddress	The I2C address of the EEPROM device
* @param[in]	memoryAddress	The address in memory to start writing to
* @param[in]	data	The byte to be written to the EEPROM
*
* @return	1 = Success
************************************************************************/

uint8_t EEPROM_write(uint16_t deviceAddress, uint16_t memoryAddress, uint8_t data)
{
	uint8_t result;
	uint8_t returnResult = 0;
	
	//Send START Condition
	result = I2C_sendStart();
	
	//Send the device Address with WRITE
	result = I2C_send(deviceAddress|TW_WRITE);

	//Send Memory Location Address to write to (High)
	result = I2C_send(memoryAddress >> 8);	//Address High

	//Send Memory Location Address to write to (Low)
	result = I2C_send((uint8_t)memoryAddress);	//Address Low

	//Write Data to EEPROM
	result = I2C_send(data);	//Data

	//Send STOP Condition
	I2C_sendStop();

	_delay_ms(10);

	returnResult = 1;
	return returnResult;
}



/**
* @brief	Read data from the EEPROM
*
* @details	This function reads data from the EEPROM from a specified address
*
* @param[in]	deviceAddress	The I2C address of the EEPROM device
* @param[in]	memoryAddress	The address in memory to read from
* @param[in]	data	The byte to be written to the EEPROM
*
* @return	The byte read from EEPROM
************************************************************************/

uint8_t EEPROM_read(uint16_t deviceAddress, uint16_t memoryAddress)
{
	
	uint8_t readData = 0;
	uint8_t result;


	//Send START Condition
	result = I2C_sendStart();

	//Send the device Address with WRITE - we need to write in order to specify the address to read from
	result = I2C_send(deviceAddress|TW_WRITE);

	//Send Memory Location Address to read from (High)
	result = I2C_send(memoryAddress >> 8);	//Address High

	//Send Memory Location Address to read from (Low)
	result = I2C_send((uint8_t)memoryAddress);	//Address Low


	//Send RESTART Condition - now we read from the memory address
	result = I2C_sendStart();


	//Send the device Address with READ
	result = I2C_send(deviceAddress|TW_READ);

	//Read the data returned by the EEPROM
	readData = I2C_read(0);	//Read Data with NO ACK  (No ACK means we're done reading)

	//Send STOP Condition
	I2C_sendStop();
	
	return readData;
	
}
