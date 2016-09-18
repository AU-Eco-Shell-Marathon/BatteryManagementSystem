/*
 * CFile1.c
 *
 * Created: 22-04-2013 15:45:31
 *  Author: Jonas
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "uart_lib.h"
#include "uart_drv.h"
#include "Frontend.h"
#include "SPI.h"
#include "power_save.h"
#include "CellAnalysis.h"
#include "CAN_transmit.h"
#include "can_drv.h"
#include "can_lib.h"
#include "BMS_Config.h"

unsigned char delay_count;

//check which cell has the lowest voltage
unsigned char get_LowestV_cell_no(void)
{
	unsigned char cellNo,i;
	unsigned int minV;
	minV = Vcell[1];
	cellNo = 1;
	for(i=2;i<=Cell_count;i++)
	{
		if(Vcell[i] < minV)
		{
			minV = Vcell[i];
			cellNo = i;
		}
	}		
	return (cellNo);
}

//check which cell has the highest voltage
unsigned char get_HighestV_cell_no(void)
{
	unsigned char cellNo,i;
	unsigned int maxV;
	maxV = Vcell[1];
	cellNo = 1;
	for(i=2;i<=Cell_count;i++)
	{
		if(Vcell[i] > maxV)
		{
			maxV = Vcell[i];
			cellNo = i;
		}
	}
	return (cellNo);
}

unsigned char get_HighestIR_cell_no(void)
{
	unsigned char cellNo,i;
	unsigned int maxIR;
	maxIR = IRcell[1];
	cellNo = 1;
	for(i=2;i<=Cell_count;i++)
	{
		if(IRcell[i] > maxIR)
		{
			maxIR = IRcell[i];
			cellNo = i;
		}
	}
	return (cellNo);
}

unsigned char get_LowestIR_cell_no(void)
{
	unsigned char cellNo,i;
	unsigned int minIR;
	minIR = IRcell[1];
	cellNo = 1;
	for(i=2;i<=Cell_count;i++)
	{
		if(IRcell[i] < minIR)
		{
			minIR = IRcell[i];
			cellNo = i;
		}
	}
	return (cellNo);
}

//get average value of cells. As a bonus pack voltage is calculated
unsigned int Avr_Cell_Voltage(void)
{
	unsigned int Avr_result;
	unsigned long Sum = 0;
	
	for(unsigned char i=1;i<=Cell_count;i++)
	{
		Sum += Vcell[i]; 
	}
	Vpack = Sum/1000;
	Avr_result = Sum / Cell_count;
	return (Avr_result);
} 

//get average value of cell IR.
unsigned int Avr_Cell_IR(void)
{
	unsigned int Avr_result;
	unsigned long Sum = 0;
	
	for(unsigned char i=1;i<=Cell_count;i++)
	{
		Sum += IRcell[i];
	}
	Avr_result = Sum / Cell_count;
	return (Avr_result);
}
//Checks for fast current changes and small margin to safety thresholds
unsigned char increase_measurement_rate(void)
{
	unsigned char warning_repport = 0;
	
	//is cell voltage close to upper limit
	if(Vcell[get_HighestV_cell_no()] > (Vcell_max-100))
	warning_repport = 1;
	
	//is cell voltage close to lower limit
	else if (Vcell[get_LowestV_cell_no()] < (Vcell_min+100))
	warning_repport = 2;
	
	//is temperature close to charge limits(and is charger connected)
	else if((Tbatt[1] > (Tcharge_max-5) || Tbatt[1] < (Tcharge_min+5)) && Charger_sense)
	warning_repport = 3;
	
	//is temperature close to discharge limits
	else if(Tbatt[1] > (Tdischarge_max-5) || Tbatt[1] < (Tdischarge_min+5))
	warning_repport = 4;
	
	//is current close to max charge level, even if discharge current is high warning is set
	else if((abs(Idischarge-Icharge)) > (Icharge_max-500))
	warning_repport = 5;
	
	//is current rapidly changing
	if(abs((signed long)Idischarge_old - Idischarge) > (Idischarge_old/4) && Idischarge > 1500)
	warning_repport = 6;
	
	//is current rapidly changing
	else if(abs((signed long)Icharge_old - Icharge) > (Icharge_old/4) && Icharge > 400)
	warning_repport = 7;
	
	return (warning_repport);
}

//Checks if any parameter exceeds safety thresholds
unsigned char Safety_check (void)
{
	unsigned int Vcell_test;
	unsigned char bit_pattern = 0;
	
	//clear errors. Will be set below if errors
	error_code_SW_protection = 0;
	//If any cell is above threshold, collect new measurement
	//if still above threshold OPEN isolation switch
	Vcell_test = get_HighestV_cell_no();
	if (Vcell[Vcell_test] > Vcell_max)
	{
		start_ADC_conversion();
		_delay_ms(1);
		collect_ADC_values();
		
		if (Vcell[Vcell_test] > Vcell_max)
		{
			//Isolate battery and set error bit
			Iso_switch_OPEN;
			error_code_SW_protection = 1;
			for(int i = 1;i<=number_of_BQ76PL536a_in_stack;i++)
			{
					//discharge cell through bleeder resistor
					bit_pattern |= (1<<(Vcell_test-1));
					balance_cell(i,bit_pattern);
			}
		}
	}
	//If any cell is below threshold, collect new measurement
	//if still below threshold OPEN isolation switch
	else
	Vcell_test = get_LowestV_cell_no();
	if (Vcell[Vcell_test] < Vcell_min)
	{
		start_ADC_conversion();
		_delay_ms(1);
		collect_ADC_values();
		if (Vcell[Vcell_test] < Vcell_min)
		{
			//Isolate battery and set error bit
			Iso_switch_OPEN;
			error_code_SW_protection = 2;
			Can_disable();
			CanTranceiver_sleep	
				
		}
	}

	//Check if battery is charging, if so check if
	//temperature is ok for charging
	else if(Charger_sense)
	{
		if(Tbatt[1] > Tcharge_max || Tbatt[1] < Tcharge_min)
		start_ADC_conversion();
		_delay_ms(1);
		collect_ADC_values();
		if(Tbatt[1] > Tcharge_max || Tbatt[1] < Tcharge_min)
		{
			//Isolate battery and set error bit
			Iso_switch_OPEN;
			error_code_SW_protection = 3;
		}
		else if (Icharge > Icharge_max)
		{
			start_ADC_conversion();
			_delay_ms(1);
			collect_ADC_values();
			if (Icharge > Icharge_max)
			{
				//Isolate battery and set error bit
				Iso_switch_OPEN;
				error_code_SW_protection = 4;
			}
		}
	}
	
	//Not charging. Check if battery temperature is ok for discharge
	else if(Tbatt[1] > Tdischarge_max || Tbatt[1] < Tdischarge_min)
	{
			start_ADC_conversion();
			_delay_ms(1);
			collect_ADC_values();			
			if(Tbatt[1] > Tdischarge_max || Tbatt[1] < Tdischarge_min)
			{
				//Isolate battery and set error bit
				Iso_switch_OPEN;
				error_code_SW_protection = 5;
			}
	}	
	else if (Idischarge > Idischarge_max)
	{
		start_ADC_conversion();
		_delay_ms(1);
		collect_ADC_values();
		if (Idischarge > Idischarge_max)
		{
			//Isolate battery and set error bit
			Iso_switch_OPEN;
			error_code_SW_protection = 6;
		}
	}
	else if (Idischarge > Idischarge_max_delayed)
	{
		delay_count++;
		if (delay_count >= (Idischarge_delay*5))
		{
			//Isolate battery and set error bit
			Iso_switch_OPEN
			error_code_SW_protection = 7;
		}
	}
	else
	{
		delay_count = 0;
	}
	
	if (error_code_SW_protection)
	CANstate = 0;
	
	return (error_code_SW_protection);
}

void measure_IR_discharge(void)
{
	for(int i=1;i<=Cell_count;i++)
	{
		//Calculate IR in mOhm*10 typecasting used to prevent overflow, and handle negative(signed) values
		IRcell[i]=((abs(((signed long)Vcell_old[i]-Vcell[i])*10000))/abs((signed long)Idischarge_old-Idischarge));
		if (Testmode)
		uart_mini_printf ("\r\nIRcell= %u \r\n",IRcell[i]);
		
		//Check if any cell variates significantly 
		if (IRcell[get_HighestIR_cell_no()] > (Avr_Cell_IR()*2))
		CAN_DTC = 1;
		else
		CAN_DTC = 0;
	}
}
//used at known SOC to change current flow by interrupting charge current
//and calculate IR. As conditions are similar each time this measurement is done, the result
//can be used to detect change over time 
unsigned char measure_IR_Force_current_change(void)
{
	unsigned int Vcell_high[Cell_count+1];
	unsigned int Vcell_low[Cell_count+1];
	unsigned int Icharge_high;
	unsigned int Icharge_low;
	
	//is charger connected and isolation switch closed?
	if (Charger_sense)
	{
		//Measure cell voltages while constant charge current 
		start_ADC_conversion();
		
		//Wait for conversion to finish
		_delay_ms(2);
		
		//Read ADC
		collect_ADC_values();
		
		//Open isolation switch to force current change
		Iso_switch_OPEN;
		
		//Place measured values in buffer for subsequent calculations
		//and creates some delay to allow relay to open
		for(int i=1;i<=Cell_count;i++)
		{
			Vcell_high[i] = Vcell[i];
		}
		Icharge_high = Icharge;
		_delay_ms(500);
		//Measure cell voltages while current is zero
		start_ADC_conversion();
		
		//wait for conversion to finish
		_delay_ms(2);
		
		//read ADC
		collect_ADC_values();
		
		//Close switch again
		Iso_switch_CLOSE;
		
		//Place measured values in buffer for subsequent calculations
		for(int i=1;i<=Cell_count;i++)
		{
			Vcell_low[i] = Vcell[i];
		}
		Icharge_low = Icharge;
		
		//wait for relay to close and voltage to climb a bit
		_delay_ms(100);
		
		//Measure cell voltages when charge current back on
		start_ADC_conversion();
		
		//wait for conversion to finish
		_delay_ms(2);
		
		//read ADC
		collect_ADC_values();
		
		//Add measured voltage
		for(int i=1;i<=Cell_count;i++)
		{
			Vcell_high[i] += Vcell[i];
		}
		Icharge_high += Icharge;
		//calculate individual cell IR in mOhm*10
		for(int i=1;i<=Cell_count;i++)
		{
			IRcell[i]=(unsigned long)(((Vcell_high[i]/2)-Vcell_low[i])*10000)/((Icharge_high/2)-Icharge_low);
		}
		return(1);
	}
	
	else
	return(0);
}

void clear_avr_arrays(void)
{
	for (int i=1;i<=Cell_count;i++)
	{
		Vcell_sum[i] = 0;
	}
	Idischarge_sum = 0;
	Icharge_sum = 0;
	Tbatt_sum = 0;
}

void BalancingAnalysis(void)
{
	unsigned int Vcell_IRcompensated[7];
	unsigned char device;
	unsigned int minV;
	unsigned int maxV;
	unsigned int avr;
	unsigned long sum;
	unsigned char balance = 0;
	//IR compensate
	//find min and max OCV is delta OCV large, balancing is needed
	Vcell_IRcompensated[1] = Vcell[1]-(((unsigned long)Icharge*IRcell[1])/10000);
	uart_mini_printf("\r\nIRcompensated voltage 1 = %u\r\n",Vcell_IRcompensated[1]);
	minV = Vcell_IRcompensated[1];
	maxV = Vcell_IRcompensated[1];
	sum = Vcell_IRcompensated[1];
	for (int i=2;i<=Cell_count;i++)
	{
		Vcell_IRcompensated[i] = Vcell[i]-(((unsigned long)Icharge*IRcell[i])/10000);
		//uart_mini_printf("\r\nIRcompensated voltage %u = %u\r\n",i,Vcell_IRcompensated[i]);
		if(Vcell_IRcompensated[i] < minV)
		{
			minV = Vcell_IRcompensated[i];
		}
		else if(Vcell_IRcompensated[i] > maxV)
		{
			maxV = Vcell_IRcompensated[i];
		}	
		sum += Vcell_IRcompensated[i];
	}	
	uart_mini_printf("\r\nminValue = %u\r\n",minV);	
	uart_mini_printf("\r\nmaxValue = %u\r\n",maxV);	
	//Check if balancing needed
	if((maxV-minV) > 10)
	{
		
		avr = sum/Cell_count;
		for(int i=1;i<=Cell_count;i++)
		{
			if(i<7)
			device = 1;
			else if(i<13)
			device = 2;
			else if(i<19)
			device = 3;
			
			if (Vcell_IRcompensated[i]>avr)
			{
				balance |= (1<<(i-1));
			}
			else
			{
				balance &= ~(1<<(i-1));
			}
			//If balancing defined for all cells of device write to device
			if(i==6 || i==12 || i==18)		
			{
				balance_cell(device,balance);
				uart_mini_printf("\r\nbalanceVar %u\r\n",balance);
			}						
		}
	}	
}

void CalculateSOC (void)
{
	unsigned int Vcell_IRcompensated[(Cell_count+1)];
	
	//Calculate SOC. Done for all cells to allow SOC history based balancing in the future
	for(int i=1;i<=Cell_count;i++)
	{
		//from calculated OCV find SOC
		Vcell_IRcompensated[i]=Vcell[i]+((((signed long)Idischarge-Icharge)*IRcell[i])/10000);
				
		//Vcell_IRcompensated, is in the range 3.0-3.22V
		if(Vcell_IRcompensated[i] < 3220)
		SOC[i] = 0;
				
		//Vcell_IRcompensated, is in the range 3.22-3.54V
		else if(Vcell_IRcompensated[i] < 3540)
		SOC[i] = (1 + ((((signed long)Vcell_IRcompensated[i]-3220)*11)/500))/2;

		//Vcell_IRcompensated, is in the range 3.54-3.68V
		else if(Vcell_IRcompensated[i] < 3680)
		SOC[i] = (4 + ((((signed long)Vcell_IRcompensated[i]-3540)*29)/1000));
				
		//Vcell_IRcompensated, is in the range 3.68-3.80V
		else if(Vcell_IRcompensated[i] < 3800)
		SOC[i] = (8 + ((((signed long)Vcell_IRcompensated[i]-3680)*258)/1000));
				
		//Vcell_IRcompensated, is in the range 3.80-3.88V
		else if(Vcell_IRcompensated[i] < 3880)
		SOC[i] = (39 + ((((signed long)Vcell_IRcompensated[i]-3800)*275)/1000));
				
		//Vcell_IRcompensated, is in the range 3.88-3.98V
		else if(Vcell_IRcompensated[i] < 3980)
		SOC[i] = (61 + ((((signed long)Vcell_IRcompensated[i]-3880)*110)/1000));
				
		//Vcell_IRcompensated, is in the range 3.98-4.02V
		else if(Vcell_IRcompensated[i] < 4020)
		SOC[i] = (72 + ((((signed long)Vcell_IRcompensated[i]-3980)*225)/1000));
				
		//Vcell_IRcompensated, is in the range 4.02-4.06V
		else if(Vcell_IRcompensated[i] < 4060)
		SOC[i] = (81 + ((((signed long)Vcell_IRcompensated[i]-4020)*75)/1000));
				
		//Vcell_IRcompensated, is in the range 4.06-4.08V
		else if(Vcell_IRcompensated[i] < 4080)
		SOC[i] = (84 + ((((signed long)Vcell_IRcompensated[i]-4060)*300)/1000));
				
		//Vcell_IRcompensated, is in the range 4.08-4.2V
		else
		SOC[i] = (90 + ((((signed long)Vcell_IRcompensated[i]-4080)*83)/1000));
				
		if(Testmode)
		uart_mini_printf ("\r\nSOC%u= %u \r\n",i,SOC[i]);
	}
	Soc_calc_counter = 0;
}			