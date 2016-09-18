/*
 * IncFile1.h
 *
 * Created: 22-04-2013 16:34:46
 *  Author: Jonas
 */ 

//Variables to hold battery and cell parameters
unsigned long Idischarge_sum;
unsigned long Icharge_sum;
unsigned long Idischarge_sum_SOC;
unsigned long Icharge_sum_SOC;
unsigned char Soc_calc_counter;
unsigned char SOC[17];
unsigned int Idischarge_old;
unsigned int Icharge_old;
unsigned int Vcell_sum[17];
unsigned int Vcell_old[17];
unsigned int IRcell[17];
unsigned char Vpack;
signed int Tbatt_sum;
unsigned char error_code_SW_protection;

//Prototypes

//check which cell has the lowest voltage
unsigned char get_LowestV_cell_no (void);

//check which cell has the highest voltage
unsigned char get_HighestV_cell_no (void);

//get average value of cells
unsigned int Avr_Cell_Voltage (void);

unsigned char increase_measurement_rate(void);
unsigned char Safety_check (void);
void measure_IR_discharge(void);
void clear_avr_arrays(void);
void BalancingAnalysis(void);
unsigned char get_LowestIR_cell_no(void);
unsigned char get_HighestIR_cell_no(void);
void CalculateSOC (void);