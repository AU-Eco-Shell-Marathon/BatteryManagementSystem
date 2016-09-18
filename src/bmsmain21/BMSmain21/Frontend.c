/*
 * BQ76PL536a.c
 *
 * Created: 25-03-2013 15:21:55
 *  Author: Jonas
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include "uart_lib.h"
#include "uart_drv.h"
#include "BMS_Config.h"
#include "SPI.h"
#include "power_save.h"
#include "RTC_internal.h"
#include "CAN_transmit.h"
#include "can_drv.h"
#include "can_lib.h"
#include "Frontend.h"

//CRC look-up table modified from(http://www.ti.com.cn/cn/lit/an/slua563b/slua563b.pdf)
unsigned char Crc_Table[256] = {
	0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
	0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
	0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
	0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
	0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
	0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
	0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
	0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
	0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
	0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
	0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
	0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
	0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
	0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
	0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
	0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
	0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
	0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
	0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
	0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
	0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
	0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
	0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
	0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
	0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
	0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
	0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
	0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
	0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
	0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
	0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3};

//Function write data to frontend. 
//Parameters: Device address, Register address, Register data 
void send_Data_and_CRC(unsigned char Dev_addr,unsigned char Reg_addr,unsigned char data)
{
	unsigned char crc = 0;
	//Add "write bit(lsb = 1)" to device address byte
	//Add values to buffer array 
	buffer[0] = (Dev_addr << 1) + 1;
	buffer[1] = Reg_addr;
	buffer[2] = data;
	
	//enable frontend SPI(SS high, as HW inverts)
	PORTB |= (1<<PB5);
	
	//CRC is calculated and creates some delay from SS low to first data
	crc = CRC_Value(buffer,3);
	
	CPU_speed_low();
	
	//write first byte at low speed, as optocouplars "narrows" first bit	
	Frontend_initial_SPI_speed;
	WriteByte(buffer[0]);
	//increase SPI speed
	Frontend_SPI_speed;
			
	//write Dev./Reg. address and data from buffer to frontend
	for (int i=1;i<3;i++)
	{
		WriteByte(buffer[i]);
	}
	//write CRC byte for error check
	WriteByte(crc);
	
	Frontend_SPI_disable;
	//Disable frontend SPI(SS low, as hw inverts)
	PORTB &= ~(1<<PB5);

	
	CPU_speed_high();
}

unsigned char receive_Data_and_CRC(unsigned char Dev_addr,unsigned char start_Addr,unsigned char byte_Count)
{
	unsigned char crc = 0;
	
		
	//enable frontend SPI(SS high, as HW inverts)
	PORTB |= (1<<PB5);
	
	//decrease speed
	CPU_speed_low();
	
	//Add "read bit(lsb=0)" to device address byte
	//Add values to buffer array
	buffer[0] = (Dev_addr<<1);
	buffer[1] = start_Addr;
	buffer[2] = byte_Count;
	

	
	Frontend_initial_SPI_speed
	//write Dev. addr.
	WriteByte(buffer[0]);
	
	Frontend_SPI_speed
	
	//send buffer, receive data 
	//Insert data in buffer for CRC check
	for(int i=1;i<(byte_Count+4);i++)
	{
		if (i<3)
		{
			//start addr.
			//and  number of bytes to be read 
			WriteByte(buffer[i]);
		}
		//is it first byte to be read with value different from 0?
		else if(i==3 || i==(byte_Count+3)) 
		{
			//decrease speed for first byte to be read
			//to compensate for MISO optocoupler delay 
			Frontend_initial_SPI_speed
			//generate clock pulses for receiving
			WriteByte(0x00);
			//fill receive buffer from index 0
			Rec_buffer[i-3] = SPDR;
			//fill buffer from index 3
			buffer[i] = SPDR;
		}			
		else
		{
			Frontend_SPI_speed
			//generate clock pulses for receiving
			WriteByte(0x00);
			//fill receive buffer from index 0
			Rec_buffer[i-3] = SPDR;
			//fill buffer from index 3
			buffer[i] = SPDR;
		}
	}
	
	Frontend_SPI_disable
	//Disable frontend SPI(SS low, as hw inverts)
	PORTB &= ~(1<<PB5);

	//increase speed
	CPU_speed_high();
	
	//Check for CRC errors
	crc = CRC_Value(buffer,(byte_Count+3));
	if (crc == buffer[(byte_Count+3)])
	{
		
		return(0);
	} 
	else
	{
		return(1);
	}
}

//Resets and Assign addresses to ASICs  
//Return number of ASICs in frontend 
unsigned char address_ASIC_stack(void)
{
	unsigned char CRC_error = 1;
	unsigned char ASIC_count = 0;
		
		//Broadcast Reset (resets all addresses)
		//ensures that following response at address 0 is from first device in stack
		send_Data_and_CRC(0x3f,0x3c,0xa5);
		_delay_ms(10);
		
		//Check ASIC SPI connection by reading address register
	while(CRC_error){
		CRC_error = receive_Data_and_CRC(0x00,0x3b,1);
		//if error, try again
		}
		if(CRC_error)
		{
			_delay_ms(1);
			CRC_error = receive_Data_and_CRC(0x00,0x3b,1);
			uart_mini_printf ("\rCRC ERROR. Please check connection to analog frontend\r\n");
		}
		
		//Assign addresses until all assigned
		while(Rec_buffer[0] == 0x00 && CRC_error == 0 && ASIC_count <= ASIC_max)
		{
			ASIC_count++;
			send_Data_and_CRC(0x00,0x3b,ASIC_count);
			//Check that assignment has been performed
			receive_Data_and_CRC(ASIC_count,0x3b,1);
			if (Rec_buffer[0] == (ASIC_count|0b10000000))
			{
				uart_mini_printf ("\rAddress successfully assigned to BQ76PL536a no.: %u \r\n",ASIC_count);
			}
			else
			{
				//Error in assigning address, warn user through UART  
				uart_mini_printf ("\rError occurred while assigning address to BQ76PL536a no.: %u\r\n",ASIC_count);
				//return 0 and exit
				return (0);
			}
			
			//Check for next unaddressed ASIC  
			CRC_error = 1;
			CRC_error = receive_Data_and_CRC(0x00,0x3b,1);
		}	
		uart_mini_printf ("\r%u Front End module(s) has been detected, connect up to %u battery cells \r\n",ASIC_count,(ASIC_count*6));		
		
		// Clear Address error flag, as address has now been assigned 
		send_Data_and_CRC(0x3f,0x20,0b10000000);
		send_Data_and_CRC(0x3f,0x20,0b00000000);
		
		//Return number of ASICs which has been addressed
		return(ASIC_count);
	
	}
	
void init_frontend(void)
{
		uart_mini_printf("Hello GGFront\r\n"); //KKKK
	    //Set PORTB directions for SPI, Isolation switch and Ext. interrupt inputs. 
		//PB4=conv_start, PB5=SS, PB7=Isolation_switch
		//PD0(INT0)=Fault_in, PD1(INT1)=Charger_sense
		DDRB |= 0b10101111;
		DDRD &= ~0b00000011;
		
		//Setup external interrupts (Rising edge, enable INT0 and INT1) 
		EICRA |= 0b00001111;
		//clear flags
		EIFR = 0;
		EIMSK |= 0b00000011;
		//Clear byte keeping track of IOregister
		IO_control = 0;
		//Reset Rec_buffer values
		for(int i=0;i<40;i++)
		{
			Rec_buffer[i]= 0x00;
		}
		
		//Clear SPI interrupt flag
		SPSR = 0x00;
		
		number_of_BQ76PL536a_in_stack = address_ASIC_stack();
		
		//turn on the current sensor
		turn_currentSense_ON();
		
		//Config CellBalancing safety timer (50Sec)
		send_Data_and_CRC(0x3f,0x33,50);
		
		//Config ADC
		send_Data_and_CRC(0x01,0x30,0b00111101);
		_delay_ms(1);
		
		//Turn on temperature measurement (vss)
		IO_control |= 0b00000010;
		send_Data_and_CRC(0x01,0x31,IO_control);
		_delay_ms(1);
		//Write protected registers, as redundant overvoltage protection
		//reacts too soon with preprogrammed threshold
		write_protected_registers();
		
		//Start conversion, so that data is available at first interrupt
		start_ADC_conversion();
}

// Calculates an 8 bit CRC checksum
// buffer is an array of received bytes, or bytes to be send 
// returns CRC byte 
unsigned char CRC_Value(unsigned char *buffer,unsigned char buffer_size)
{
		unsigned char crc = 0;
		int temp = 0;
		for (int i = 0; i < buffer_size; i++)
		{
			temp = crc ^ buffer[i];
			crc = Crc_Table[temp];
		}
		return crc;
}

//start conversion (all frontend devices at once)
void start_ADC_conversion(void)
{	
	unsigned char IOstatus;
	//Turn on temperature measurement (vss)
	//IO_control |= 0b00000010;
	//send_Data_and_CRC(0x01,0x31,IO_control);
	//start conversion
	send_Data_and_CRC(0x3f,0x34,0x01);
}

void Precharge_and_IsolationSwitch_close (unsigned int delay)
{
	IO_control |= 0b01000000;
	//close precharge relay connected to General I/O of bq76pl536a (device 1)
	send_Data_and_CRC(0x01,0x31,IO_control);
	//wait for period specified by parameter
	for (unsigned int i=0;i<delay;i++)
	_delay_ms(1);
	//Ensure that the frontend is not keeping the Isolationswitch from Closing
	Clear_Alerts_and_Faults();
	//Close switch
	Iso_switch_CLOSE
	//wait for Isolation Switch to close, then open precharge relay
	_delay_ms(1000);
	IO_control &= ~0b01000000;
	send_Data_and_CRC(0x01,0x31,IO_control);
}

void collect_ADC_values(void)
{
	unsigned char CRC_error = 1;
	unsigned char n = 1;
	unsigned char arrayN;
	unsigned int test;
	
	//Turn off temperature measurement(vss), to save power
	//IO_control &= ~0b00000010;
	//send_Data_and_CRC(0x01,0x31,IO_control);
		
	//Convert Vcell and Tbatt values, for all BQ76PL536a devices in stack 
	for (int x=0;x<number_of_BQ76PL536a_in_stack;x++)
	{
		//n indicates which cell data to convert(start value) 
		// n += x*6;
		if(x == 1)
		n = 7;
		else if(x == 2)
		n = 13;
		else
		n = 1;
		//Ensure conversion has finished by reading DRDY bit
		CRC_error = receive_Data_and_CRC((x+1),0x00,1);
		//uart_mini_printf ("\r\n  DRDY = %u\r\n",Rec_buffer[0] & 0b00000001);
		//uart_mini_printf ("\r\n  CRCerror = %u\r\n",CRC_error);
		if (CRC_error == 0 && (Rec_buffer[0] & 0b00000001))
		{
			//Location of first Vcell byte in Rec_buffer
			arrayN = 0x03;
			//Collect raw data from frontend
			CRC_error = receive_Data_and_CRC((x+1),0x00,18);
			//uart_mini_printf ("\r\n  CRCerror = %u\r\n",CRC_error);
			//if no CRC errors, convert data 
			if(CRC_error == 0)
			{
				//Convert raw Vcell values to mV
				//place in array (Vcell1 placed at Vcell[1] etc.) 
				for(int i=0;i<6;i++)
				{
					//Calculate mV, typecasting to prevent overflow 
					Vcell[n] = ((unsigned long)(Rec_buffer[arrayN]<<8) + Rec_buffer[arrayN+1])*6250/16383;
					
					//print values(for test purpose)
					//uart_mini_printf ("\r Vcell%u = %u\r\n",n,Vcell[n]);
					
					//Increment pointer variables
					arrayN += 2;
					n++;
				}
				
				//convert temperature and current for lowest BQ76PL536 in stack
				if(x == 0)
				{
					//GPAI ADC input raw value converted to mA
					Idischarge = (Rec_buffer[1]<<8) + Rec_buffer[2];
					
					if(Idischarge < Idis_offset)
					{
						Idischarge = 0;
					}
					else
					{
						Idischarge = (unsigned long)(Idischarge - Idis_offset)*35/10;
					}
					
					//Print for test
					//uart_mini_printf ("\r\n Idischarge 1 = %u\r\n",Idischarge);
					
					//TS1 ADC input raw value converted to mA
					Icharge = (Rec_buffer[0x0f]<<8) + Rec_buffer[0x10];  
					
					if(Icharge < Ich_offset)
					{
						Icharge = 0;
					}
					else
					{
						Icharge = (unsigned long)(Icharge - Ich_offset)*1480/1000;
					}
					//Print for test
					//uart_mini_printf ("\r\n Icharge = %u\r\n",Icharge);
					
					//TS2 ADC input raw value converted to °C
					Tbatt[1] = (((unsigned int)(Rec_buffer[0x11]<<8) + Rec_buffer[0x12])/104)-Tbatt1_offset;
					//test = (Rec_buffer[0x11]<<8) + Rec_buffer[0x12];
					//Print for test
					//uart_mini_printf ("\r Tbatt1 raw= %u\r\n",test);
				}
				else
				{
					
					//TS1 ADC input raw value converted to °C
					Tbatt[x*2] = ((unsigned long)(Rec_buffer[0x0f]<<8) + Rec_buffer[0x10]);
					//Print for test
					//uart_mini_printf ("\r Tbatt%u = %u\r\n",(x*2),Tbatt[x*2]);
					
					//TS2 ADC input raw value converted to °C
					Tbatt[x*2+1] = ((unsigned long)(Rec_buffer[0x11]<<8) + Rec_buffer[0x12]);
					//uart_mini_printf ("\r Tbatt%u = %u\r\n",(x*2+1),Tbatt[x*2+1]);
					
				}
			}
		}
		//if communication was lost set Vcell to zero to ensure current is interrupted
		else if (CRC_error)
		{
			//place in array (Vcell1 placed at Vcell[1] etc.)
			for(int i=0;i<=Cell_count;i++)
			{
				//Set values to zero
				Vcell[n] = 0;
			}
		}
	}
}


void Collect_Alerts_and_Faults (void)
{
	unsigned char CRC_error = 1;
	
	//Collect , for all BQ76PL536a devices in stack
	for (int x=1;x<=number_of_BQ76PL536a_in_stack;x++)
	{
		CRC_error = receive_Data_and_CRC((x),0x20,4);
		if(CRC_error)
		{
			receive_Data_and_CRC((x+1),0x20,4);
		}
		//place collected data in dedicated arrays
		Alerts[x] = Rec_buffer[0];
		Faults[x] = Rec_buffer[1];
		COV_Faults[x] = Rec_buffer[2];
		CUV_Faults[x] = Rec_buffer[3];
	}
}


void Clear_Alerts_and_Faults (void)
{
		for (int i=1;i<=number_of_BQ76PL536a_in_stack;i++)
		{
			// Clear Alerts 
			send_Data_and_CRC(i,0x20,0xFF);
			send_Data_and_CRC(i,0x20,0x00);
			// Clear Faults
			send_Data_and_CRC(i,0x21,0b00111111);
			send_Data_and_CRC(i,0x21,0x00);
		}
}

void turn_currentSense_ON (void)
{
	// Change status of IO register
	IO_control |= 0b10000000;
	// Turn AUX on, but keep the rest unchanged
	send_Data_and_CRC(0x01,0x31,IO_control);
}

void turn_currentSense_off (void)
{
	// Change status of IO register
	IO_control &= ~0b10000000;
	// Turn AUX on, but keep the rest unchanged
	send_Data_and_CRC(0x01,0x31,IO_control);
}

void balance_cell(unsigned char device_no, unsigned char bit_pattern)
{
	send_Data_and_CRC(device_no,0x32,bit_pattern);
}


void frontend_sleep(void)
{
	//Ensure Isolation switch is open, as protection will be off
	Iso_switch_OPEN
	
	//put to sleep and disable current and temperature sensor supply
	//Broadcast to all devices
	send_Data_and_CRC(0x3f,0x31,0b00000100);
	//After this the sleep alert bit must be cleared to reduce power
	Clear_Alerts_and_Faults();
}

void write_protected_registers(void)
{
	
	//write protected registers
	//broadcast to all devices
	//Setup ADC mux for GPIO and bandgab reference, conversion time 6us
	send_Data_and_CRC(0x01,0x3a,0x35);
	send_Data_and_CRC(0x01,0x40,0b01000000);
	
	//Overwrite COV register as fault triggers at 4100mV even though 
	//it is programmed for 4200mV (set to (theoretically) 4250mV)
	send_Data_and_CRC(0x01,0x3a,0x35);
	send_Data_and_CRC(0x01,0x42,0x2e);
	send_Data_and_CRC(0x01,0x3a,0x35);
	send_Data_and_CRC(0x01,0x43,0x8a);
	
	//Rewrite CUV as this funbction is also used at EPROM errors(2900mV)
	send_Data_and_CRC(0x01,0x3a,0x35);
	send_Data_and_CRC(0x01,0x44,0x15);	
	send_Data_and_CRC(0x01,0x3a,0x35);
	send_Data_and_CRC(0x01,0x45,0x8f);
	
	//Rewrite overtemperature as this function is also used at EPROM errors
	send_Data_and_CRC(0x01,0x3a,0x35);
	//60 degrees max for TS2, 19A for TS1
	send_Data_and_CRC(0x01,0x46,0x5B);
	send_Data_and_CRC(0x01,0x3a,0x35);
	send_Data_and_CRC(0x01,0x47,0x03);
}

unsigned char handle_Alerts_and_Faults(void)
{
	Collect_Alerts_and_Faults();
	for (int x=1;x<=number_of_BQ76PL536a_in_stack;x++)
	{
		//Handle Alerts
		switch (Alerts[x])
		{
			//Address not assigned 
			case 0b10000000:
				address_ASIC_stack();
				break;
				
			//protected registers corrupted	
			case 0b01000000:
				write_protected_registers();
				Clear_Alerts_and_Faults();
				break;
				
			//OTP-EPROM corrupted
			case 0b00100000:
				write_protected_registers();
				Clear_Alerts_and_Faults();
				break;
				
			//Alert output asserted for test
			case 0b00010000:
				Clear_Alerts_and_Faults();
				break;
				
			//Frontend ASIC too hot
			case 0b00001000: 
				//try putting it to sleep
				frontend_sleep();
				//Isolate battery
				Iso_switch_OPEN
				break;
				
			//Sleep mode was activated
			case 0b00000100:
				//Ensure battery is isolated
				Iso_switch_OPEN
				break;
				
			//Battery overtemperature has been detected
			case 0b00000010:
				//Check if this is a real condition or triggered by mistake
				if (Tbatt[x] > Tdischarge_max)
				{
					//Ensure battery is isolated
					Iso_switch_OPEN
				}
				//else try clearing it, and see if it happens again
				else 
				{
					Clear_Alerts_and_Faults();
				}
				
				break;
				
			//Charge current too high according to frontend	
			case 0b00000001:
				//Check if this is a real condition or triggered by mistake
				if (Icharge > Icharge_max)
				{
					//Ensure battery is isolated
					Iso_switch_OPEN
				}
				//else try clearing it, and see if it happens again
				else
				{
					Clear_Alerts_and_Faults();
				}
				break;
		}	
		
		//Handle Faults
		switch (Faults[x])
		{
			//Consistency check failed
			//Reset stack using the address function
			case 0b00100000:
			address_ASIC_stack();
			Clear_Alerts_and_Faults();
			break;
			
			//Fault asserted for test
			case 0b00010000:
			//just clear it
			Clear_Alerts_and_Faults();
			break;
			
			//Power on reset
			case 0b00001000:
			//protected registers must be reset
			write_protected_registers();
			Clear_Alerts_and_Faults();
			break;
			
			//CRC error occurred 
			case 0b00000100:
			//mainly critical if it happened during protected register write
			//write them again to be sure
			write_protected_registers();
			Clear_Alerts_and_Faults();
			break;
			
			//Cell undervoltage occurred 
			case 0b00000010:
			//Check if really true 
			if (Vcell[get_LowestV_cell_no()] < Vcell_min)
			{
				//disable everything and bring power consumption to minimum
				Iso_switch_OPEN
				_delay_ms(10000);
				start_ADC_conversion();
				_delay_ms(1);
				collect_ADC_values();
				if (Vcell[get_LowestV_cell_no()] > Vcell_min)
				{
					Precharge_and_IsolationSwitch_close(500);
				}
				else
				{
					//disable frontend including current and temp sensor
					frontend_sleep();
					Frontend_SPI_disable
						Can_disable();
						CanTranceiver_sleep
					//prepare CPU for sleep and put to sleep
					set_sleep_mode(SLEEP_MODE_PWR_DOWN);
					sleep_enable();
					__asm("sei");
					sleep_cpu();
					
					//Lines below are executed right after interrupt routine at wake-up
					sleep_disable();
					//Charger is connected after a deep discharge, warn user
					uart_mini_printf("\r\nCharger was connected after a deep discharge, please decrease charging current to C/10\r\n");
					//at wakeup clear faults
					Clear_Alerts_and_Faults();
					//turn current sensor back on
					turn_currentSense_ON();
					//init RTC again
					rtc_init_internal();
				}
				
				
				
				
			}
			//try clearing
			else
			Clear_Alerts_and_Faults();
			break;
			
			//Cell overvoltage
			case 0b00000001:
			//Check which cell is too high and enable bleeder resistor across it
			receive_Data_and_CRC(x,0x22,1);
			balance_cell(x,Rec_buffer[0]);
			//Ensure switch is open
			Iso_switch_OPEN
			_delay_ms(10000);
			start_ADC_conversion();
			_delay_ms(1);
			collect_ADC_values();
			if (Vcell[get_HighestV_cell_no()] < Vcell_max)
			{
				Precharge_and_IsolationSwitch_close(500);
			}
			
			break;
		}
					
	}	
}	
