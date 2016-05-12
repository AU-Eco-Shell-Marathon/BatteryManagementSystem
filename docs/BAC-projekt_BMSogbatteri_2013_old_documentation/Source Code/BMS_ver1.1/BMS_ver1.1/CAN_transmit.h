/*
 * CAN_transmit.h
 *
 * Created: 12-06-2013 14:07:37
 *  Author: Jonas
 */ 

unsigned char CANstate;
unsigned char CANflags;
unsigned int CAN_sec_tick;
unsigned char CAN_DTC;
unsigned long CAN_BattEnergyIn;
unsigned long CAN_BattEnergyOut;

#define CanTranceiver_enable {DDRF &= ~(1<<PF3);}
#define CanTranceiver_sleep {PORTF |= (1<<PF3); DDRF|= (1<<PF3);}

void transmit_all_CAN_data (void);