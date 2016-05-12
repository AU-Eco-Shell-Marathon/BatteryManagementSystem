/*
 * ITAMS_Group4
 *
 * Created: 21-03-2013 12:36:10
 * Author: Jonas Nyborg
 */ 

unsigned long Fram_count;
unsigned char Rec_ram[45];
unsigned int Fram_Sec;
unsigned char data_buffer[45];
//setup SPI: Master mode, MSB first, SCK idle low
//Double speed not enabled
#define FRAM_SPI_speed     SPCR = 0x51; 
#define FRAM_enable {PORTB &= ~(1<<PB0);}
#define FRAM_disable {PORTB |= (1<<PB0);}	

void FRAM_init(void);
void WriteFRAM(unsigned long address, unsigned char *ram_buffer, unsigned char buffer_size);
void ReadFRAM(unsigned long address,unsigned char size);
void FRAM_sleep (void);
void LogData_Transmit_to_USB (void);
unsigned char update_DataLog (void);
void pack_Ram_data (void);