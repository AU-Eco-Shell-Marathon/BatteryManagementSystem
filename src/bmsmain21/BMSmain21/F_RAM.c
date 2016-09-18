/*
 * 
 *
 * Created: 21-03-2013 12:36:10
 * Author: Jonas Nyborg
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "F_RAM.h"
#include "SPI.h"
#include "uart_lib.h"
#include "uart_drv.h"
#include "CAN_transmit.h"
#include "Frontend.h"
#include "CellAnalysis.h"
#include "BMS_Config.h"

unsigned char size;

void FRAM_init(void)
{
	//Setup port direction
	DDRB |= 0b00000111;
	
	//Change to high SPI speed
	FRAM_SPI_speed
	//enable F-RAM (SS low)
	FRAM_enable
	_delay_us(1);
	//send write enable command(must be send to perform write)
	WriteByte(0b00000110);
	FRAM_disable
	_delay_us(1);
	FRAM_enable
	//send write status register command
	WriteByte(0b00000110);
	
	//send control register value 
	//(disable HW write protect, no protected memory addresses)
	WriteByte(0b00000000);
	
	//Disable F-RAM (SS high)
	FRAM_disable
	_delay_us(1);
	
	//Collect and set counter for keeping track of clusters
	ReadFRAM(0,2);
	Fram_count = (Rec_ram[0]<<8)+Rec_ram[1];
	uart_mini_printf ("\r\nFram count%u",Fram_count);
	//Collect and update seconds
	//Get size of log data clusters(placed in size)
	pack_Ram_data();
	//Read the last cluster of data
	ReadFRAM(((Fram_count-1)*size+3),size);
	//Extract seconds
	Fram_Sec = (Rec_ram[0]<<8)+Rec_ram[1];
	uart_mini_printf ("\r\nFram Sec%u",Fram_Sec);
	//Put to sleep
	//FRAM_sleep();
}



void WriteFRAM(unsigned long address, unsigned char *ram_buffer, unsigned char buffer_size)
{
	unsigned char addressMSB;
	unsigned char addressMLSB;
	unsigned char addressLSB;
	
	// Change to high SPI speed
	FRAM_SPI_speed
	
		
	FRAM_enable
	//send write enable command(must be send to perform write)
	WriteByte(0b00000110);
	FRAM_disable
	
	// Enable F-RAM (SS low)
	FRAM_enable
	
	// Convert address to 3 bytes
	addressLSB = address;
	addressMLSB = address >> 8;
	addressMSB = address >> 16;
	//uart_mini_printf ("\r\n addMlsb: %u\r\n",addressMLSB);
	//uart_mini_printf ("\r\n addMsb: %u\r\n",addressMSB);
	
	// Send write command(must be send to perform write)
	WriteByte(0b00000010);
	
	// Send addressMSB
	WriteByte(addressMSB);
		
	// Send addressMLSB
	WriteByte(addressMLSB);
	
	// Send addressLSB
	WriteByte(addressLSB);
	
	for (int i=0;i<buffer_size;i++)
	{
		// Send data
		WriteByte(ram_buffer[i]);
	}
	
	// Disable F-RAM (SS high)
	FRAM_disable
}

void ReadFRAM(unsigned long address,unsigned char size)
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
	addressMLSB = address >> 8;
	addressMSB = address >> 16;
	
	// Send read command
	WriteByte(0b00000011);
	
	// Send address to be read	
	WriteByte(addressMSB);
	WriteByte(addressMLSB);
	WriteByte(addressLSB);
	
	for(int i=0;i<size;i++)	
	{
		// Send "dummy byte" (to generate 8 clock periods)
		WriteByte(0x00);
		// Read the received data byte
		Rec_ram[i]=SPDR;
	}
	
	// Disable F-RAM (SS high)
	FRAM_disable
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

unsigned char update_DataLog (void)
{
	unsigned char Fram_array[2];
	
	//Place Fram count in F-RAM, to keep track of clusters in memory
	Fram_array[0] = Fram_count>>8;
	Fram_array[1] = Fram_count;
	WriteFRAM(0,Fram_array,2);
	
	//prepare data to be logged and place in F-RAM and get pointer to packed array
	pack_Ram_data();
	WriteFRAM((Fram_count*size+3),data_buffer,size);
	
	//increment Fram count to indicate that data cluster was added.
	Fram_count++;
	
	return(size);
}

void LogData_Transmit_to_USB (void)
{
	unsigned int tick_count;
	unsigned int Vcell_temp;
	unsigned char buffer_count;
	signed char Tbatt_temp;
	signed int Ibatt_temp;

	//Performed for each log data cluster
	for (unsigned int i=0;i<Fram_count;i++)
	{
		//Read the cluster of data
		ReadFRAM((i*size+3),size);
		
		//Time
		tick_count = (Rec_ram[0]<<8)+Rec_ram[1];
		uart_mini_printf ("%u",tick_count);
		
		//Cell count
		uart_mini_printf (",%u",Rec_ram[2]);
		
		//Cell voltages
		buffer_count = 3;
		for (int i=1;i<=Cell_count;i++)
		{
			Vcell_temp = (Rec_ram[buffer_count]<<8)+Rec_ram[buffer_count+1];
			uart_mini_printf (",%u",Vcell_temp);
			buffer_count += 2;
		}
		
		//battery current [mA/10]
		Ibatt_temp = (Rec_ram[buffer_count]<<8)+Rec_ram[buffer_count+1];
		uart_mini_printf (",%d",Ibatt_temp);
		buffer_count += 2;
		
		//Temperature
		uart_mini_printf (",%d",Rec_ram[buffer_count]);
		buffer_count ++;
		 		
		//SOC
		uart_mini_printf (",%u",Rec_ram[buffer_count]);
		buffer_count ++;
			
		//SOH(for future use)
		uart_mini_printf (",%u",Rec_ram[buffer_count]);
		buffer_count ++;
		
		//Error code
		uart_mini_printf (",%u\r\n",Rec_ram[buffer_count]);
	}
}

void pack_Ram_data (void)
{
	unsigned char buffer_count;
	signed int	temperary;
	
	//Set size count to 0, incremented at every byte write
	size=0;
	
	//Place time passed since turn-on
	data_buffer[0] = Fram_Sec>>8;	size ++;
	data_buffer[1] = Fram_Sec;		size ++;
	
	//holds number of cells
	data_buffer[2] = Cell_count;		size ++;
	//Place cell voltages
	buffer_count = 3;
	for (int i=1;i<=Cell_count;i++)
	{
		//Place MSB Vcell
		data_buffer[buffer_count] = Vcell[i]>>8;	size ++;
		buffer_count++;
		//Place LSB Vcell
		data_buffer[buffer_count] = Vcell[i];		size ++;
		buffer_count++;
	}
	
	//Place battery current [mA/10](to allow signed value contained in 2 bytes)
	temperary = ((signed long)Idischarge-Icharge)/10;
	data_buffer[buffer_count] = temperary>>8;	size ++;
	buffer_count++;
	data_buffer[buffer_count] = temperary;		size ++;
	buffer_count++;
	
	//Place Temperature
	data_buffer[buffer_count] = Tbatt[1];		size ++;
	buffer_count++;
	
	//Place SOC (for cell 1 as all cells are expected to be within a 2% range)
	data_buffer[buffer_count] = SOC[1];		size ++;
	buffer_count++;
	
	//Place SOH(for future use)
	data_buffer[buffer_count] = 100;		size ++;
	buffer_count++;
	
	//Place error code
	data_buffer[buffer_count] = error_code_SW_protection;	size ++;
}