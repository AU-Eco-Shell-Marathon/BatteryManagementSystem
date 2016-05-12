/*
 * Frontend.h
 *
 * Created: 25-03-2013 15:25:55
 *  Author: Jonas
 */ 


#include <avr/io.h>

//Defines related to Front End
#define Charger_sense (PIND & (1<<PIND1))
#define Iso_switch_CLOSE {PORTB |=(1<<PB7);batt_isolated = 0;}
#define Iso_switch_OPEN {PORTB &=~(1<<PB7); batt_isolated = 1;}

//CPOL = 0, CPHA = 1, SPI master
#define Frontend_SPI_speed     SPCR = 0b01010111;

//CPOL = 0, CPHA = 1, SPI master
#define Frontend_initial_SPI_speed     SPCR = 0b01010111;

//SPI disable
#define Frontend_SPI_disable		SPCR &= ~(1<<SPE); 

//System variables
unsigned char buffer[40];
unsigned char Rec_buffer[40];
unsigned char number_of_BQ76PL536a_in_stack;
unsigned char batt_isolated;

//variables to hold converted ADC values
unsigned int Vcell[17];
signed char Tbatt[6];
unsigned int Idischarge;
unsigned int Icharge;
unsigned char Alerts[4];
unsigned char Faults[4];
unsigned char COV_Faults[4];
unsigned char CUV_Faults[4];
unsigned char IO_control;
unsigned char balance_register;

//prototypes
void init_frontend(void);
void start_ADC_conversion();
void Precharge_and_IsolationSwitch_close (unsigned int delay);
void collect_ADC_values();
void Collect_Alerts_and_Faults (void);
void Clear_Alerts_and_Faults (void);
void turn_currentSense_ON (void);
void balance_cell(unsigned char device_no, unsigned char bit_pattern);
unsigned char handle_Alerts_and_Faults(void);
unsigned char CRC_Value(unsigned char *buffer,unsigned char buffer_size);