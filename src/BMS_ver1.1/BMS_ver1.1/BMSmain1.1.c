/*
 * BMS_ver1.1.c
 *
 * Created: 26-02-2013 16:34:21
 *  Author: Jonas
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include "CellAnalysis.h"
#include "RTC_internal.h"
#include "uart_lib.h"
#include "uart_drv.h"
#include "can_drv.h"
#include "can_lib.h"
#include "F_RAM.h"
#include "Frontend.h"
#include "CAN_transmit.h"
#include "BMS_Config.h"

//Status flags
unsigned char charger_connected = 0;
unsigned char high_measurement_rate = 0;
unsigned char IR_done = 0;
//counter variables
unsigned char balance_counter = 59;
unsigned char LogInterval_count = 0;
//Variable holding number of performed measurements
unsigned char number_of_measurements = 0;
unsigned char AFE_init_error = 0;
//unsigned char charge_pause = 0;
void done_every_second(void)
{
	unsigned int Idischarge_result;
	unsigned int Icharge_result;
	unsigned char safety_status = 0;
	
	//collect new battery and cell data
	collect_ADC_values();
	number_of_measurements++;
	
	//turn on CAN transceiver as it takes some time
	//for it to wake from sleep(better doing it now than using delays)
	PORTF &= ~(1<<PF3);
	
	//if high measurement rate has been activated for the passed second
	//the average must be found, else result is equal to collected ADC values
	if(high_measurement_rate)
	{
		Idischarge_result = (Idischarge_sum + Idischarge)/number_of_measurements;
		Icharge_result = (Icharge_sum + Icharge)/number_of_measurements;
		clear_avr_arrays();
	}
	
	else
	{
		Idischarge_result = Idischarge;
		Icharge_result = Icharge;
	}
	number_of_measurements = 0;
	
	//If test mode, print values
	if(Testmode)
	{
		for(int c=1;c<=Cell_count;c++)
		{
			uart_mini_printf("\rCell%u = %umV\r\n",c,Vcell[c]);
		}
	
		
		uart_mini_printf("\rIdischarge = %dmA \r\n",Idischarge_result);
		uart_mini_printf("\rIcharge = %dmA \r\n",Icharge_result);
		uart_mini_printf("\rTbatt1 = %d \r\n",Tbatt[1]);
		//uart_mini_printf ("\rTbatt2 = %d \r\n",Tbatt[2]);
		//uart_mini_printf ("\rTbatt3 = %d \r\n",Tbatt[3]);
	}
	
	//Check if there is reason to increase measurement rate
	high_measurement_rate = increase_measurement_rate();
	
	//if there is reason for increasing measurement rate
	//there is also reason to check if safety limits has been exceeded
	if(high_measurement_rate)
	{
		safety_status = Safety_check();
		
		//if discharge current has changed dramatically there is foundation for IR calculation
		if(high_measurement_rate==6)
		{
			measure_IR_discharge();
		}
	}
		
	//values used for comparison
	Idischarge_old = Idischarge_result;
	Icharge_old = Icharge_result;
	for (int i=1;i<=Cell_count;i++)
	{
		Vcell_old[i]=Vcell[i];
	}
	
	//if 20Sec has passed it is time to do SOC calculations
	if(Soc_calc_counter == 20)
	{
		CalculateSOC();		
	}
	//Increment SOC interval counter
	Soc_calc_counter++;
	
	//Charger has been connected, see if still connected, and act accordingly  
	//if(charger_connected)
	//{
		
		//Charger still connected
		//No charger sense is used now
		//if(Charger_sense)
		if(Icharge_result>(Idischarge_result+1000))//If charge current above 1000mA it is most likely a charger
		{
			PORTE &= ~(1<<PE4);
			//Set CAN status indicator bytes
			CANstate = 1;
			CANflags = 1;
			//Is highests cell voltage above balancing threshold
			if(Vcell[get_HighestV_cell_no()] > BalanceThreshold && balance_counter==60)
			{
				//If IR analysis not done, do it
				if(IR_done == 0)
				{
				//check IR to allow IR compensation
				measure_IR_Force_current_change();
				IR_done = 1;
				}
				//Perform balancing
				BalancingAnalysis();
				balance_counter=0;
			}
			balance_counter++;
		}		
				
		//Charger was removed
		else
		{
			//turn off charging LED
			PORTE |= (1<<PE4);
				
			//Reset charge indicator
			charger_connected = 0;
			IR_done = 0;
		}
		
		if(Testmode)
		uart_mini_printf("\r\n batt_isolated = %d \r\n",batt_isolated);
		//see if battery is Isolated as result of overvoltage
		//if(batt_isolated==1)
		//{
			////Has voltage decreased to safe level, if so reapply charger
			//if((Vcell[get_HighestV_cell_no()] < (Vcell_max-40)&& charge_pause)
			//{
				//Precharge_and_IsolationSwitch_close(1000);
			//}
		//}
	//}	
	
	if (LogInterval_count == Log_Interval)
	{
		//Store data in external memory to create data log
		update_DataLog();
		LogInterval_count = 0;
	}
	LogInterval_count++;
		
	//Transmit CAN
	transmit_all_CAN_data();	
	
	//Start ADC to enable collection of new values
	//at next interrupt
	start_ADC_conversion();
}

//done every 200mS, but not when done_every_second function is called
void done_every_200mS(void)
{
	//is high measurement rate active?
	if(high_measurement_rate)
	{
		//Collect cell and battery data(conversion was started at last interrupt)
		collect_ADC_values();
		Safety_check();
		for (int i=1;i<=Cell_count;i++)
		{
			Vcell_sum[i] += Vcell[i];
		}
		Icharge_sum += Icharge;
		Idischarge_sum += Idischarge;
		number_of_measurements++;
		start_ADC_conversion();
	}
}

//Things to be done with specific time interval is called here
//This interrupt appears every 200mS
ISR(TIMER2_COMP_vect)
{
	//disable sleep to avoid unintended sleep or interrupts
	//CPU_disable_sleep();
	//write to interrupttimer register and check busy flag
	//before reentering sleep. This ensures that one Timer osc clock period
	//has passed, which is necesary for correct wake-up on next interrupt
	OCR2A = 50;
	__asm("cli");

	
	
	//Update mSec, Sec, Min, etc.
		rtc_milliseconds+=200;             //-- Increments milliseconds
		
		if (rtc_milliseconds == 1000)
		{
			rtc_milliseconds = 0;
			rtc_seconds++;              //-- Increments seconds
			Fram_Sec++;
			CAN_sec_tick++;
			//one second has passed since last CAN transmission
			//prepare data, check safety, and send via CAN
			done_every_second();

			if (rtc_seconds == 60)
			{
				rtc_seconds = 0;
				rtc_minutes++;          //-- Increments minutes
				
				if (rtc_minutes == 60)
				{
					rtc_minutes = 0;
					rtc_hours++;        //-- Increments hours
			
					
					if (rtc_hours == 24)
					{
						rtc_hours = 0;
						rtc_days++;     //-- Increments days
					}
				}
			}
		}
		else
		{
			done_every_200mS();
		}
		
	//avoid missing next interrupt	
	while ( ASSR & (1<<OCR2UB))//--wait while busy
	{}
}

//Executed when charger is connected
//furthermore it can wake up the BMS after deep sleep
ISR(INT1_vect)
{
	__asm("cli");
	if(Testmode)
	{
		uart_mini_printf("\r\ncharger has been connected\r\n");	
	}
	//turn on charge LED indicator
	PORTE &= ~(1<<PE4);
	charger_connected = 1;
	
}	

//Executes when the front end detects an Fault or Alarm
ISR(INT0_vect)
{
	__asm("cli");
	handle_Alerts_and_Faults();
	CANstate = 0;
	if(Testmode)
	uart_mini_printf("\r\nFault: %u Alarm: %u\r\n",Faults[1],Alerts[1]);
}

//Executes on UART RX complete
ISR(USART1_RX_vect)
{
	switch(uart_getchar())
	{
		//Log request
		case '1':
			if(Fram_count)
			LogData_Transmit_to_USB();
			else
			uart_mini_printf("\r\nLog is empty\r\n");
		break;
		//Reset Log
		case '2':
			Fram_count = 0;
			Fram_Sec = 0;
			uart_mini_printf("Log was cleared\r\n");
		break;
		//Send log size
		case '3':
			uart_mini_printf("%u\r\n",Fram_count);
		break;
		//return error to allow test of connection
		default:
			uart_mini_printf("Unknown request\r\n");
		break;
	} 
}

void main(void)
{
	__asm("cli");
	// Init UART
	// Init UART-1 at baudrate: 38400, data bits: 8, stop bit: 1
	Uart_select(UART_1);
	uart_init(CONF_8BIT_NOPAR_1STOP,38400);
	//enable RX interrupt
	UCSR1B |=0b10000000;
	
	//turn on LED to show user the BMS was turned on
	DDRE |= (1<<PE4);
	PORTE &= ~(1<<PE4);
	
	//init frontend: Establish SPI connection look for number of extension modules, Address modules
	init_frontend();
	if(number_of_BQ76PL536a_in_stack<AFE_count)
	{
		AFE_init_error=1;
	}
	//Clear arrays and variables used for Cell Analysis
	clear_avr_arrays();
	Idischarge_sum_SOC = 0;
	Icharge_sum_SOC = 0;
	Soc_calc_counter = 20;
	//Setup F-ram (puts the ic to sleep after init, so wake up before use)
	FRAM_init();
	
	if(Vcell[get_HighestV_cell_no()] > Vcell_max)
	{
		//Activate bleeder resistors if cell V's are too high.
		balance_cell(0x3f, 0b00111111);
		_delay_ms(4000);
	}
	
	//Setup timer as Real Time Counter with external osc.
	CAN_sec_tick = 0;
	rtc_init_internal();
	
	//ensure IR is 0 before measurement is performed
		for (int i=1;i<=Cell_count;i++)
		{
			IRcell[i]=0;
		}
		
	//Clear Power on Reset error etc. in frontend
	Clear_Alerts_and_Faults();
	Collect_Alerts_and_Faults ();
	uart_mini_printf ("\r\n Alerts and faults: %u and %u \r\n",Alerts[1],Faults[1]);
	
	
	
	//Close the isolation switch
	if (AFE_init_error==0)
	{
		Precharge_and_IsolationSwitch_close(1000);
	}
	
	
	//Set CAN state bytes
	CANstate = 2;
	CANflags = 2;
	
	//turn off LED
	PORTE |= (1<<PE4);
	
	//Init CAN
	DDRF |= (1<<PF3);
	CAN_PORT_DIR &= ~(1<<CAN_INPUT_PIN );
	CAN_PORT_DIR &= ~(1<<CAN_OUTPUT_PIN);
	CAN_PORT_OUT |=  (1<<CAN_INPUT_PIN );
	CAN_PORT_OUT |=  (1<<CAN_OUTPUT_PIN);
	can_fixed_baudrate(125); //parameter not used
	can_clear_all_mob(); 
	

	while(1)
	{	
		//balance_cell(1,0b00000001);				
		//Go back to sleep right after interrupt routine has been executed
		//If usb connected go to IDLE only to allow wakeup on UART interrupt
		if(USB_sense)
		{
			set_sleep_mode(SLEEP_MODE_IDLE);
		}
		else 
		{
			set_sleep_mode(SLEEP_MODE_PWR_SAVE);
		}
		sleep_enable();
		__asm("sei");
		sleep_cpu();
		//this line is executed right after interrupt routine at wake-up 
		sleep_disable();
	}
}