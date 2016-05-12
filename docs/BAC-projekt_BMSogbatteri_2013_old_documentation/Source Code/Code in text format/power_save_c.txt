/*
 * CFile1.c
 *
 * Created: 16-04-2013 14:13:42
 *  Author: Jonas
 */ 
#include <avr/io.h>
#include <util/delay.h>

void CPU_speed_low(void)
{
	//enable change of Clk prescaler
	CLKPR = 0b10000000;
	//Set prescaling to  (resulting in cpu speed 1MHz)
	CLKPR = 0b00000011;
	_delay_ms(1);
}


void CPU_speed_high(void)
{
	//enable change of Clk prescaler
	CLKPR = 0b10000000;
	//Set prescaling to 1 (resulting in cpu speed 8MHz)
	CLKPR = 0b00000000;	
	_delay_ms(1);
}


