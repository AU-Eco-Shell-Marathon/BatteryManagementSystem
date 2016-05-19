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
//this affects log speed only, not measurement execution speed.
#define Log_Interval 5

//number of cells connected
#define Cell_count 12

//number of Frontend ics connected
#define AFE_count 2

//Battery design capacity[mAh]
#define capacity 2650// was 3300

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

//Max charge current[mA]
#define Icharge_max 8000 // was 15000

//Max discharge current[ma]
#define Idischarge_max 25000// was 55000

//Max discharge current, delayed interrupt 
#define Idischarge_max_delayed 20000 // was 50000

//delay in seconds before current is interrupted(applies for delayed limit only) 
#define Idischarge_delay 10


//Technical configuration below, consider changes twice

//Sensor offset
#define Idis_offset 10
#define Ich_offset 691
#define Tbatt1_offset 20

//maximum number of connected ASICs in Analog Front End
#define ASIC_max 3

