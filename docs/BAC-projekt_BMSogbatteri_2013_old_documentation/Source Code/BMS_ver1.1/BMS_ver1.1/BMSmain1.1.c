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

//Variable holding number of performed measurements
unsigned char number_of_measurements = 0;


void done_every_second(void)
{
	unsigned int Vcell_result[17];
	unsigned int Idischarge_result;
	unsigned int Icharge_result;
	unsigned char safety_status = 0;
	unsigned int Vcell_IRcompensated[17];
	signed long SOCcurrent;
	unsigned char Idischarge_msb;
	unsigned char Idischarge_lsb; 
	unsigned char Fram_msb;
	unsigned char Fram_lsb;
	
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
		for (int i=1;i<=Cell_count;i++)
		{
			Vcell_result[i] = (Vcell_sum[i]+Vcell[i])/number_of_measurements;
		}
		Idischarge_result = (Idischarge_sum + Idischarge)/number_of_measurements;
		Icharge_result = (Icharge_sum + Icharge)/number_of_measurements;
		clear_avr_arrays();
	}
	
	else
	{
		for (int i=1;i<=Cell_count;i++)
		{
			Vcell_result[i] = Vcell[i];
		}
		Idischarge_result = Idischarge;
		Icharge_result = Icharge;
	}
	number_of_measurements = 0;
	
	//If test mode, print values
	if(Testmode)
	{
		for(int i=1;i<=Cell_count;i++)
		{
			uart_mini_printf ("\rCell nr. %u = %u \r\n",i,Vcell_result[i]);
		}

		uart_mini_printf ("\rIdischarge = %u \r\n",Idischarge_result);
		uart_mini_printf ("\rIcharge = %u \r\n",Icharge_result);
		uart_mini_printf ("\rTbatt1 = %d \r\n",Tbatt[1]);
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
	
	//accumulate current for SOC measurement
	Idischarge_sum_SOC += Idischarge_result;
	Icharge_sum_SOC += Icharge_result;
	Soc_calc_counter++;
	
	//if a minute has passed it is time to do SOC calculations
	if(Soc_calc_counter == 60)
	{
		//Calculate SOC. Done for all cells to allow SOC history based balancing in the future
		for(int i=1;i<=Cell_count;i++)
		{
			//if voltage is high base SOC on voltage measurement
			//SOC format is[%]*10 to avoid rounding error drift over time when colomb counting is used
			if(Vcell[i]>3980)
			{
				//from calculated OCV find SOC
				Vcell_IRcompensated[i]=Vcell[i]+((((signed long)Idischarge-Icharge)*IRcell[i])/10000);
				
				//if the compensated value is below range for voltage based SOC, do nothing
				if(Vcell_IRcompensated < 3980)
				SOC[i]=SOC[i];
				
				//Vcell_IRcompensated, is in the range 3.98-4.02V
				else if(Vcell_IRcompensated < 4020)
				SOC[i] = (72 + ((((signed long)Vcell_IRcompensated[i]-3980)*225)/1000))*10;
				
				//Vcell_IRcompensated, is in the range 4.02-4.06V
				else if(Vcell_IRcompensated < 4060)
				SOC[i] = (81 + ((((signed long)Vcell_IRcompensated[i]-4020)*75)/1000))*10;
				
				//Vcell_IRcompensated, is in the range 4.06-4.08V
				else if(Vcell_IRcompensated < 4080)
				SOC[i] = (84 + ((((signed long)Vcell_IRcompensated[i]-4060)*300)/1000))*10;
				
				//Vcell_IRcompensated, is in the range 4.08-4.2V
				else
				SOC[i] = (90 + ((((signed long)Vcell_IRcompensated[i]-4080)*83)/1000))*10;
			}
			//is the SOC very near to or below zero, set it to zero
			//Here Isolation switch must have opened to ensure no current is running
			//this means that no IR compensation is needed
			else if(Vcell[i] < 3100 && batt_isolated==1)
			{
				SOC[i] = 0;
			}
			//Otherwise base the SOC on coulomb counting(book keeping method)
			else
			{
				//find average and direction of current
				SOCcurrent = (Idischarge_sum_SOC-Icharge_sum_SOC)/Soc_calc_counter;
				
				//Ah in/out of battery, converted to % and added to previus SOC
				SOC[i] = (((unsigned int)10*SOC[i])-((SOCcurrent*100/6)/3300));
			}
			
			if(Testmode)
			uart_mini_printf ("\r\nSOC%u= %u \r\n",i,SOC[i]);
		}
		Idischarge_sum_SOC = 0;
		Icharge_sum_SOC = 0;
		Soc_calc_counter = 0;
		////Write discharge current to f-ram 
		//Idischarge_msb = (Idischarge_result>>8);
		//Idischarge_lsb = Idischarge_result;
		//WriteFRAM(Fram_count,Idischarge_msb);
		//Fram_count++;
		//WriteFRAM(Fram_count,Idischarge_lsb);
		//Fram_count++;
		//Fram_msb = (Fram_count>>8);
		//Fram_lsb = Fram_count;
		//WriteFRAM(0,Fram_msb);
		//WriteFRAM(1,Fram_lsb);
	}
	
	//Charger has been connected, see if still connected, and act accordingly  
	if(charger_connected)
	{
		
		//Charger still connected
		if (Charger_sense)
		{
			
			//disable balancing to measure actual cell voltage when analysing
			//change to broadcast when number of devices>1
			balance_cell(0x01,0x00);
			
			//Set CAN status indicator bytes
			CANstate = 1;
			CANflags = 1;
			//Is highests cell voltage above balancing threshold
			if(Vcell[get_HighestV_cell_no()] > BalanceThreshold)
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
			}
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
		
		uart_mini_printf("\r\n batt_isolated = %d \r\n",batt_isolated);
		//see if battery is Isolated as result of overvoltage
		if(batt_isolated==1)
		{
			//Has voltage decreased to safe level, if so reapply charger
			if(Vcell[get_HighestV_cell_no()] < (Vcell_max-5))
			{
				Precharge_and_IsolationSwitch_close(1000);
			}
		}
	}	
		
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
	uart_mini_printf("\r\ncharger has been connected\r\n");	
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
	uart_mini_printf("\r\nFault: %u Alarm: %u\r\n",Faults[1],Alerts[1]);
}

void main(void)
{
	unsigned char Fram_count_msb;
	unsigned char Fram_count_lsb;
	__asm("cli");
	// Init UART
	// Init UART-1 at baudrate: 38400, data bits: 8, stop bit: 1
	Uart_select(UART_1);
	uart_init(CONF_8BIT_NOPAR_1STOP,38400);
	
	//turn on LED to show user the BMS was turned on
	DDRE |= (1<<PE4);
	PORTE &= ~(1<<PE4);
	
	//init frontend: Establish SPI connection look for number of extension modules, Address modules
	init_frontend();
	
	//Clear arrays and variables used for Cell Analysis
	clear_avr_arrays();
	Idischarge_sum_SOC = 0;
	Icharge_sum_SOC = 0;
	Soc_calc_counter = 0;
	//Setup F-ram (puts the ic to sleep after init, so wake up before use)
	FRAM_init();
	
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
	uart_mini_printf ("\r\n Alerts and faults: %u and %u\r\n",Alerts[1],Faults[1]);
	
	//Close the isolation switch
	Precharge_and_IsolationSwitch_close(1000);
	
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
		//Go back to sleep right after interrupt routine has been executed
		set_sleep_mode(SLEEP_MODE_PWR_SAVE);
		sleep_enable();
		__asm("sei");
		sleep_cpu();
		//this line is executed right after interrupt routine at wake-up 
		sleep_disable();
	}
}