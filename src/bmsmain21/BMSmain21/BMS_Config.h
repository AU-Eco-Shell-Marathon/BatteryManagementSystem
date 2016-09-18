/*
 * BMS_Config.h
 *
 * Created: 11-05-2013 12:40:12
 *  Author: Jonas
 */ 

//if set to 1 values are presented to the user via USB, 
//if not needed set to 0 to reduce power consumption 	
#define Testmode 1

//Time between data logging[s]
#define Log_Interval 5

//number of cells connected
#define Cell_count 12

//Battery design capacity[mAh]
#define capacity 3300

//Lower threshold[mV]
#define Vcell_min 3000

//Upper threshold[mV]
#define Vcell_max 4190

//Max temperature (charging)
#define Tcharge_max 45

//Min temperature (charging)
#define Tcharge_min 0

//Max temperature (discharging) 
#define Tdischarge_max 60

//Min temperature (discharging)
#define Tdischarge_min -15

//Balancing threshold
#define BalanceThreshold 3800

//Max charge current
#define Icharge_max 15000

//Max discharge current
#define Idischarge_max 55000

//Max discharge current, delayed interrupt 
#define Idischarge_max_delayed 50000

//delay in seconds before current is interrupted 
#define Idischarge_delay 10


//Technical configuration below, consider changes twice

//Sensor offset
#define Idis_offset 10
#define Ich_offset 691
#define Tbatt1_offset 23

//maximum number of connected ASICs in Analog Front End
#define ASIC_max 3

