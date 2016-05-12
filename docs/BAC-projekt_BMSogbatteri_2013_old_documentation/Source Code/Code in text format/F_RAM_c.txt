/*
 * 
 *
 * Created: 21-03-2013 12:36:10
 * Author: Jonas Nyborg
 */ 

#include <avr/io.h>
#include "F_RAM.h"
#include "SPI.h"
#include "uart_lib.h"
#include "uart_drv.h"

void FRAM_init(void)
{
	//Setup port direction
	DDRB |= 0b00000111;
	
	//Change to high SPI speed
	FRAM_SPI_speed
	//enable F-RAM (SS low)
	FRAM_enable
	
	//send write enable command(must be send to perform write)
	WriteByte(0b00000110);
	
	//send write status register command
	WriteByte(0b00000110);
	
	//send control register value 
	//(disable HW write protect, no protected memory addresses)
	WriteByte(0b00000000);
	
	//Disable F-RAM (SS high)
	FRAM_disable
	__asm("NOP");
	__asm("NOP");
	
	//Put to sleep
	FRAM_sleep();
}



void WriteFRAM(unsigned long address, unsigned char data)
{
	unsigned char addressMSB;
	unsigned char addressMLSB;
	unsigned char addressLSB;
	
	// Change to high SPI speed
	FRAM_SPI_speed
			
	// Enable F-RAM (SS low)
	FRAM_enable
	
	// Convert address to 3 bytes
	addressLSB = address;
	addressMLSB = address>>8;
	addressMSB = address>>16;
	
	// Send write enable command(must be send to perform write)
	WriteByte(0b00000110);
	
	// Send write command(must be send to perform write)
	WriteByte(0b00000010);
	
	// Send addressMSB
	WriteByte(addressMSB);
		
	// Send addressMLSB
	WriteByte(addressMLSB);
	
	// Send addressLSB
	WriteByte(addressLSB);
	
	// Send data
	WriteByte(data);
	
	// Disable F-RAM (SS high)
	FRAM_disable
}

unsigned char ReadFRAM(unsigned long address)
{
	unsigned char addressMSB;
	unsigned char addressMLSB;
	unsigned char addressLSB;
		
	// Change to high SPI speed
	FRAM_SPI_speed
	
	// Enable F-RAM (SS low)
	FRAM_enable
	
	// Convert address to 3 bytes
	addressLSB = address;
	addressMLSB = address>>8;
	addressMSB = address>>16;
	
	// Send read command
	WriteByte(0b00000011);
	
	// Send address to be read	
	WriteByte(addressMSB);
	WriteByte(addressMLSB);
	WriteByte(addressLSB);
		
	// Send "dummy byte" (to generate 8 clock periods)
	WriteByte(0x00);
	
	// Disable F-RAM (SS high)
	FRAM_disable
	
	// Read and return the received data byte
	return(SPDR);
}

void FRAM_sleep (void)
{
	// Change to high SPI speed
	FRAM_SPI_speed
	// Enable F-RAM (SS low)
	FRAM_enable
	// Send sleep command, be aware wake-up takes 400uS
	WriteByte(0b10111001);
	// Disable F-RAM (SS high)
	FRAM_disable
}