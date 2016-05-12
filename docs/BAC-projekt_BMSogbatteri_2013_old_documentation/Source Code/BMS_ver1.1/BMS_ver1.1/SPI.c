/*
 * SPI.c
 *
 * Created: 25-03-2013 15:17:00
 *  Author: Jonas
 */ 

#include <avr/io.h>

//Send byte function shared by Write-/ReadFRAM and BMS functions
void WriteByte(unsigned char data)
{
	unsigned char temperary;
	
	// Send data byte
	SPDR = data;
	// Await data sent
	while(!(SPSR&(1<<SPIF)))
{}
	// Clear SPIF flag
	temperary = SPSR;
	temperary = SPDR;
}