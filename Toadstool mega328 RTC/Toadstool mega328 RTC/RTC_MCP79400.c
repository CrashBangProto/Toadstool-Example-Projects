/*
 * @file	RTC_MCP79400.c
 *
 *  ------------------------------------
 *  @author	Andrew Retallack, Crash-Bang Prototyping
 *			www.crash-bang.com
 *  @date	30/05/2015
 *
 */ 

#include "RTC_MCP79400.h"


volatile uint8_t settingBackupBat = 1;	//Is Backup Battery Enabled?
volatile uint8_t setting24Hour = 1;		//Do we use 24-hour format?



/**
* @brief	
*
* @details	
*			
*
* @param[in]	
*
* @return	
************************************************************************/


/**
* @brief		Initialise the RTC
*
* @details		Initialise the RTC: Oscillator running. 
*				Record the hour format (24-hr vs 12-hr) - does not actually set this in the RTC
*				Record whether backup battery connected - does not actually set this in the RTC
*
*
* @param[in]	deviceAddress	The I2C address of the RTC device
*				is24Hour		Are we using 24-hour format (1=24 / 0 = 12)
*				isBackupBat		Is a backup battery connected?
*
* @return
************************************************************************/
uint8_t RTC_Init(uint8_t deviceAddress, uint8_t is24Hour, uint8_t isBackupBat)
{
	
	uint8_t tempVar = 0;
	
	settingBackupBat = isBackupBat;
	setting24Hour = is24Hour;
	
#if DEBUGLEVEL > 1
	UART_writeString("\r\n\r\n--------RTC_Init-------\r\n");
#endif	

	
	//Check Oscillator and Backup Bat settings
	#if DEBUGLEVEL > 2
	UART_writeString("\r\n\r\n---Read Osc and VBATEN---\r\n");
	#endif

	tempVar = RTC_read(deviceAddress, MCP794_RTCWKDAY);

	//If Backup Battery setting on module is not same as setting passed, update
	if ((tempVar & (1<<MCP794_VBATEN)) != isBackupBat)
	{
		#if DEBUGLEVEL > 2
		UART_writeString("\r\n---Correcting Backup Battery setting to: ");
		UART_printDecimal(isBackupBat);
		UART_writeString("---\r\n");
		#endif

		if (isBackupBat == 0)
		{
			tempVar &= ~(1<<MCP794_VBATEN);	//Disable the backup battery bit
		}
		else
		{
			tempVar |= (1<<MCP794_VBATEN);	//Enable the backup battery bit
		}
		
		RTC_write(deviceAddress, MCP794_RTCWKDAY, tempVar);	//Write setting back
	}
	
	if ( (tempVar & (1<<MCP794_OSCRUN)) == 0)	//Oscillator is not running - start it
	{

		#if DEBUGLEVEL > 2
		UART_writeString("\r\n\r\n---Start Osc---\r\n");
		#endif
		RTC_write(deviceAddress, MCP794_CONTROL, 0);	//Ensure external Osc disabled
		RTC_write(deviceAddress, MCP794_RTCSEC, 10 | (1<<MCP794_ST));	//Reset seconds to 10 (arbitrary) and start oscillator
	}
	
	_delay_ms(10);	//Give oscillator time to start up, then check it's running
	
	#if DEBUGLEVEL > 2
	UART_writeString("\r\n\r\n---Read Osc---\r\n");
	#endif
	
	tempVar = RTC_read(deviceAddress, MCP794_RTCWKDAY);
	if ( (tempVar & (1<<MCP794_OSCRUN)) == 0)	//Oscillator is not running
	{
		
		#if DEBUGLEVEL > 2
		UART_writeString("TempVar= ");
		UART_printDecimal(tempVar,0);
		UART_writeString("\r\n");
		#endif
		return 0;	//Error: Oscillator didn't start
	}
	
	return 1;	//All OK, oscillator started
}

/**
* @brief	Set the time on the the RTC
*
* @details	This function sets the time
*
* @param[in]	deviceAddress	The I2C address of the RTC device
* @param[in]	setXxxxx		The Value of each Date/Time Element to set
*
* @return	1 = Success
************************************************************************/

uint8_t RTC_SetTime(uint8_t deviceAddress, uint16_t setYear, uint8_t setMonth, uint8_t setDay, uint8_t setWeekDay, uint8_t setHour, uint8_t isHourPM, uint8_t setMinutes, uint8_t setSeconds)
{
	uint8_t result;
	uint8_t returnResult = 0;
	uint8_t sendData = 0;
	
#if DEBUGLEVEL > 1
	UART_writeString("\r\n\r\n--------RTC_SetTime-------\r\n");
#endif


#if DEBUGLEVEL > 2
	UART_writeString("---Disable Osc---\r\n");
#endif

	//Disable Oscillator
	RTC_write(deviceAddress, MCP794_RTCSEC, 0);  //This also results in seconds going to zero - does not matter as we set them later
	
	//Send START Condition
	result = I2C_sendStart();


#if DEBUGLEVEL > 2
	UART_writeString("---Year---\r\n");
#endif
	//---Year
	result = I2C_send(deviceAddress|TW_WRITE);  //Send the device Address with WRITE
	result = I2C_send(MCP794_RTCYEAR);			//Register
	//Only interested in last 2 digits of year:
	if (setYear > 99)
	{
		sendData = setYear % 100;
	}
	else
	{
		sendData = setYear;
	}
	result = I2C_send(decToBcd(sendData));


#if DEBUGLEVEL > 2
	UART_writeString("---Month---\r\n");
#endif
	//---Month
	result = I2C_sendStart();
	result = I2C_send(deviceAddress|TW_WRITE);  //Send the device Address with WRITE
	result = I2C_send(MCP794_RTCMTH);			//Register
	result = I2C_send(decToBcd(setMonth));		//Value


#if DEBUGLEVEL > 2
	UART_writeString("---Date---\r\n");
#endif
	//---Date
	result = I2C_sendStart();
	result = I2C_send(deviceAddress|TW_WRITE);  //Send the device Address with WRITE
	result = I2C_send(MCP794_RTCDATE);			//Register
	result = I2C_send(decToBcd(setDay));		//Value


#if DEBUGLEVEL > 2
	UART_writeString("---Hour---\r\n");
#endif
	//---Hour
	result = I2C_sendStart();
	result = I2C_send(deviceAddress|TW_WRITE);  //Send the device Address with WRITE
	result = I2C_send(MCP794_RTCHOUR);			//Register
	if (setting24Hour)	//If 24 Hour format:
	{
		result = I2C_send(decToBcd(setHour));		//Value - include 12/24-hour format
	}
	else  //12-hour format
	{
		if (setHour > 12)	//Check the passed hour is actually 12-hr format
		{
			sendData = setHour - 12;
		}
		result = I2C_send(decToBcd(sendData) | (1 << MCP794_12_24) | (isHourPM << MCP794_AM_PM));		//Value - include 12/24-hour format
	}
	

#if DEBUGLEVEL > 2
	UART_writeString("---Minute---\r\n");
#endif
	
	//---Minutes
	result = I2C_sendStart();
	result = I2C_send(deviceAddress|TW_WRITE);  //Send the device Address with WRITE
	result = I2C_send(MCP794_RTCMIN);			//Register
	result = I2C_send(decToBcd(setMinutes));		//Value
	

#if DEBUGLEVEL > 2
	UART_writeString("---Second---\r\n");
#endif
	//---Seconds
	result = I2C_sendStart();
	result = I2C_send(deviceAddress|TW_WRITE);  //Send the device Address with WRITE
	result = I2C_send(MCP794_RTCSEC);			//Register
	result = I2C_send(decToBcd(setSeconds) | (1<<MCP794_ST));		//Value - Also start oscillator


#if DEBUGLEVEL > 2
	UART_writeString("---SendStop---\r\n");
#endif
	//Send STOP Condition
	I2C_sendStop();

	_delay_ms(100);	//Wait for oscilator to start

	returnResult = result;
	return returnResult;
}



/**
* @brief	Read the time on the the RTC
*
* @details	This function reads the time
*
* @param[in]	deviceAddress	The I2C address of the RTC device
* @param[in]	getXxxxx		Pointer to each Date/Time Element to retrieve
*
* @return	1 = Success
************************************************************************/

uint8_t RTC_GetTime(uint8_t deviceAddress, uint16_t *getYear, uint8_t *getMonth, uint8_t *getDay, uint8_t *getWeekDay, uint8_t *getHour, uint8_t *isHourPM, uint8_t *getMinutes, uint8_t *getSeconds)
{
	uint8_t result;
	uint8_t returnResult = 0;
	uint8_t readData = 0;
	
	//We need to retrieve the entire date/time in one read, so as to prevent time roll-overs and errors
	
	//Send START Condition.
	result = I2C_sendStart();
	
	//---Year
	#if DEBUGLEVEL > 2
	UART_writeString("---Year---\r\n");
	#endif
	result = I2C_send(deviceAddress|TW_WRITE);  //Send the device Address with WRITE
	result = I2C_send(MCP794_RTCYEAR);			//Register
	result = I2C_sendStart();					//Restart - now we're going to do a read operation
	result = I2C_send(deviceAddress|TW_READ);  //Send the device Address with READ
	readData = I2C_read(0);
	*getYear = bcdToDec(readData);
	


	//---Month
	#if DEBUGLEVEL > 2
	UART_writeString("---Month---\r\n");
	#endif
	result = I2C_sendStart();					//Restart - now we're going to do a Write operation
	result = I2C_send(deviceAddress|TW_WRITE);  //Send the device Address with WRITE
	result = I2C_send(MCP794_RTCMTH);			//Register
	result = I2C_sendStart();					//Restart - now we're going to do a read operation
	result = I2C_send(deviceAddress|TW_READ);	//Send the device Address with READ
	readData = I2C_read(0) & MCP794_MASK_Month;	//Read the data
	*getMonth = bcdToDec(readData);				//Convert from BCD to Integer


	//---Date
	#if DEBUGLEVEL > 2
	UART_writeString("---Day---\r\n");
	#endif
	result = I2C_sendStart();
	result = I2C_send(deviceAddress|TW_WRITE);  //Send the device Address with WRITE
	result = I2C_send(MCP794_RTCDATE);			//Register
	result = I2C_sendStart();					//Restart - now we're going to do a read operation
	result = I2C_send(deviceAddress|TW_READ);	//Send the device Address with READ
	readData = I2C_read(0);						//Read the data
	*getDay = bcdToDec(readData);				//Convert from BCD to Integer


	//---Hour
	#if DEBUGLEVEL > 2
	UART_writeString("---Hour---\r\n");
	#endif
	result = I2C_sendStart();
	result = I2C_send(deviceAddress|TW_WRITE);  //Send the device Address with WRITE
	result = I2C_send(MCP794_RTCHOUR);			//Register
	result = I2C_sendStart();					//Restart - now we're going to do a read operation
	result = I2C_send(deviceAddress|TW_READ);	//Send the device Address with READ
	readData = I2C_read(0);						//Read the data
	
	if (readData & (1<<MCP794_12_24))	//Is 12-hour format
	{
		UART_writeString("\r\n12 Hour Masked = ");
		UART_printDecimal(readData & MCP794_MASK_12Hour,0);
		UART_writeString("\r\n12 Hour Converted = ");
		UART_printDecimal(bcdToDec(readData & MCP794_MASK_12Hour),0);
		UART_writeString("\r\n");
		*isHourPM = readData & MCP794_AM_PM;	//Mask out the AM/PM
		*getHour = bcdToDec(readData & MCP794_MASK_12Hour);	//Mask out the 12-hour time
	}
	else
	{
		*isHourPM = 0;
		*getHour = bcdToDec(readData & MCP794_MASK_24Hour);	//Mask out the 24-hour time
	}


	//---Minutes
	#if DEBUGLEVEL > 2
	UART_writeString("---Minutes---\r\n");
	#endif
	result = I2C_sendStart();					//Restart - now we're going to do a Write operation
	result = I2C_send(deviceAddress|TW_WRITE);  //Send the device Address with WRITE
	result = I2C_send(MCP794_RTCMIN);			//Register
	result = I2C_sendStart();					//Restart - now we're going to do a read operation
	result = I2C_send(deviceAddress|TW_READ);	//Send the device Address with READ
	readData = I2C_read(0);						//Read the data
	*getMinutes = bcdToDec(readData);				//Convert from BCD to Integer
	

	//---Seconds
	#if DEBUGLEVEL > 2
	UART_writeString("---Seconds---\r\n");
	#endif
	result = I2C_sendStart();					//Restart - now we're going to do a Write operation
	result = I2C_send(deviceAddress|TW_WRITE);  //Send the device Address with WRITE
	result = I2C_send(MCP794_RTCSEC);			//Register
	result = I2C_sendStart();					//Restart - now we're going to do a read operation
	result = I2C_send(deviceAddress|TW_READ);	//Send the device Address with READ
	readData = I2C_read(0);						//Read the data
	*getSeconds = bcdToDec(readData & MCP794_MASK_Second);			//Convert from BCD to Integer


	//Send STOP Condition
	I2C_sendStop();

	_delay_ms(10);	//Wait for oscilator to start

	returnResult = result;
	return returnResult;
}



/**
* @brief	Write a byte to the RTC
*
* @details	This function writes a byte to the RTC to a specified register
*
* @param[in]	deviceAddress	The I2C address of the RTC device
* @param[in]	registerAddress	The register to write to
* @pram[in]	data			The byte to be written to the register
*
* @return	1 = Success
************************************************************************/

uint8_t RTC_write(uint8_t deviceAddress, uint8_t registerAddress, uint8_t data)
{
	uint8_t result;
	uint8_t returnResult = 0;
	
	//Send START Condition
	result = I2C_sendStart();
	
	//Send the device Address with WRITE
	result = I2C_send(deviceAddress|TW_WRITE);

	//Send Register Address to write to
	result = I2C_send(registerAddress);	

	//Write byte to register
	result = I2C_send(data);	//Data

	//Send STOP Condition
	I2C_sendStop();

	_delay_ms(10);

	returnResult = 1;
	return returnResult;
}



/**
* @brief	Read data from the RTC
*
* @details	This function reads data from the RTC from a specified register
*
* @param[in]	deviceAddress	The I2C address of the RTC device
* @param[in]	registerAddress	The register to read from
*
* @return	The byte read from RTC
************************************************************************/

uint8_t RTC_read(uint8_t deviceAddress, uint8_t registerAddress)
{
	
	uint8_t readData = 0;
	uint8_t result;


	//Send START Condition
	result = I2C_sendStart();

	//Send the device Address with WRITE - we need to write in order to specify the register to read from
	result = I2C_send(deviceAddress|TW_WRITE);

	//Send Register Address to read from
	result = I2C_send(registerAddress);	


	//Send RESTART Condition - now we read from the memory address
	result = I2C_sendStart();


	//Send the device Address with READ
	result = I2C_send(deviceAddress|TW_READ);

	//Read the data returned by the RTC
	readData = I2C_read(0);	//Read Data with NO ACK  (No ACK means we're done reading)

	//Send STOP Condition
	I2C_sendStop();
	
	return readData;
	
}


/**
* @brief	Convert Decimal to Binary-Coded Decimal
*
* @details	This function converts a decimal number to BCD
*
* @param[in]	val	The decimal number (max 99)
*
* @return	byte containing the BCD value
************************************************************************/
uint8_t decToBcd(uint8_t val)
{
	return ( (val / 10 * 16) + (val % 10) );
}


/**
* @brief	Convert Binary-Coded Decimal to Decimal
*
* @details	This function converts a BCD byte to a decimal number
*
* @param[in]	val	The decimal number (max 99)
*
* @return	byte containing the BCD value
************************************************************************/
uint8_t bcdToDec(uint8_t val)
{
	return ( (val / 16 * 10) + (val % 16) );
}
