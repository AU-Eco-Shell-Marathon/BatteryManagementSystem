/*
 * RTC_internal.c
 *
 * Created: 26-02-2013 16:36:44
 *  Author: Jonas
 */ 

#include <avr/io.h>
#include "RTC_internal.h"
#include "uart_lib.h"
#include "uart_drv.h"


void rtc_init_internal(void)
{
	
	unsigned int wait;

	//init procedure following AT90CAN128 guidelines to avoid register corruption at oscillator shift
	//Waiting Xtal to stabilize after a power is applied
	for (wait=0;wait<0xFFFF;wait++);
	
	//disable timer interrupts
	TIMSK2 &= ~0b00000011;	
	
	//Init RTC clock for 32.768 KHZ external XTAL		
	ASSR  =  (ASSR & ~(1<<EXCLK)) | (1<<AS2); 	
			
	//Timer 2 cleared & not running
	TCCR2A=0; TCNT2=0; OCR2A=0;     
	
	//CTC mode
	TCCR2A |= (1<<WGM21);
	
	//compare register set for match every 200 msec.			
	OCR2A = 50;
	
	//prescaler 64, start count						
	TCCR2A |= 0x05;				
	
	//wait while busy	
	while ( ASSR & ( (1<<TCN2UB) | (1<<OCR2UB) | (1<<TCR2UB) ) )
	{}
	
	//Clear Output_Compare Interrupt-flags
	TIFR2 |=  (1<<OCF2A);			
	
	  //Reset Time variables
	  rtc_milliseconds = 0;
	  rtc_seconds      = 0;
	  rtc_minutes      = 0;
	  rtc_hours        = 0;
	  rtc_days         = 0;
	  
	//Enable Compare interrupt  
	TIMSK2 |= (1<<OCIE2A);			
}	  
