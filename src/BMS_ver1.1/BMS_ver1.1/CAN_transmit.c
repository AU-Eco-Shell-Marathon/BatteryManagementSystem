/*
 * CAN_transmit.c
 *
 * Created: 10-06-2013 13:24:43
 *  Author: Jonas
 */ 

#include <avr/io.h>
#include "CellAnalysis.h"
#include "uart_drv.h"
#include "can_drv.h"
#include "can_lib.h"
#include "CAN_transmit.h"
#include "Frontend.h"
#include "BMS_Config.h"

//Set Name and SW version to be send
const char BMS_Name[8] = "IHA BMS1";
const char SW_Ver[8] = "SW: 1.10";

void transmit_all_CAN_data (void)
{
	//variables to hold message objects
	unsigned char Addr622[6];
	unsigned char Addr623[6];
	unsigned char Addr624[6];
	unsigned char Addr625[8];
	unsigned char Addr626[7];
	unsigned char Addr627[6];
	unsigned char Addr628[6];
	st_cmd_t message;
	
	////fill data into Addr622 array
	Addr622[0] = CANstate; 
	Addr622[1] = CAN_sec_tick>>8;
	Addr622[2] = CAN_sec_tick;
	Addr622[3]= CANflags;
	Addr622[4] = CAN_DTC;
	Addr622[5] = CAN_DTC;
	
	////fill data into Addr623 array
	////ensure Vpack is found 
	Avr_Cell_Voltage();
	Addr623[0] = Vpack>>8;
	Addr623[1] = Vpack;
	unsigned char Vlow = get_LowestV_cell_no();
	Addr623[2] = Vcell[Vlow];
	Addr623[3]= Vlow;	
	unsigned char VHigh = get_HighestV_cell_no(); 
	Addr623[4] = Vcell[VHigh];
	Addr623[5] = VHigh;

	////fill data into Addr624 array
	int current_A = (Idischarge-Icharge)/1000;
	Addr624[0] = current_A >> 8;
	Addr624[1] = current_A;
	Addr624[2] = 0; 
	Addr624[3]= 15;
	Addr624[4] = 0;
	Addr624[5] = 50;	  
	
	//fill data into Addr625 array
	Addr625[0] = 0xFF;
	Addr625[1] = 0xFF;
	Addr625[2] = 0xFF;
	Addr625[3]= 0xFF;
	Addr625[4] = 0xFF;
	Addr625[5] = 0xFF;
	Addr625[6] = 0xFF;
	Addr625[7] = 0xFF;
	
	//fill data into Addr626 array
	Addr626[0] = SOC[1];
	
	unsigned int DOD= (unsigned int)(capacity/1000 * (100-(SOC[1]/10)))/100;//not exact have to be improved
	
	Addr626[1] = DOD>>8;
	Addr626[2] = DOD;
	Addr626[3] = capacity>>8;
	Addr626[4] = capacity;
	Addr626[5] = 0xFF;
	Addr626[6] = 0xFF;
	
	//fill data into Addr627 array
	Addr627[0] = Tbatt[1];
	Addr627[1] = 0xFF;
	Addr627[2] = 0xFF;
	Addr627[3]= 0xFF;
	Addr627[4] = 0xFF;
	Addr627[5] = 0xFF;

	
	//fill data into Addr627 array
	Addr628[0] = 0xFF;
	Addr628[1] = 0xFF;
	unsigned char IRlow = get_LowestIR_cell_no();
	Addr628[2] = IRcell[IRlow];
	Addr628[3]= IRlow;
	unsigned char IRHigh = get_HighestIR_cell_no();
	Addr628[4] = IRcell[IRHigh];
	Addr628[5] = IRHigh;
	
	//enable the CAN features of AT90CAN128
	Can_enable();
	
	//Clear must be performed before placing new data in message object buffers
	can_clear_all_mob();
	
	
	//Fill data into message objects
	message.id.std = 0x620;
	message.dlc = 8;
	message.pt_data = BMS_Name;
	message.ctrl.ide = 0;
	message.cmd = CMD_TX_DATA;
	// --- Enable Tx
	while(can_cmd(&message) != CAN_CMD_ACCEPTED);

	message.id.std = 0x621;
	message.dlc = 8;
	message.pt_data = SW_Ver;
	// --- Enable Tx
	while(can_cmd(&message) != CAN_CMD_ACCEPTED);

	message.id.std = 0x622;
	message.dlc = 6;
	message.pt_data = Addr622;
	// --- Enable Tx
	while(can_cmd(&message) != CAN_CMD_ACCEPTED);

	message.id.std = 0x623;
	message.dlc = 6;
	message.pt_data = Addr623;
	// --- Enable Tx
	while(can_cmd(&message) != CAN_CMD_ACCEPTED);

	message.id.std = 0x624;
	message.dlc = 6;
	message.pt_data = Addr624;
	// --- Enable Tx
	while(can_cmd(&message) != CAN_CMD_ACCEPTED);

	message.id.std = 0x625;
	message.dlc = 8;
	message.pt_data = Addr625;
	// --- Enable Tx
	while(can_cmd(&message) != CAN_CMD_ACCEPTED);

	message.id.std = 0x626;
	message.dlc = 7;
	message.pt_data = Addr626;
	// --- Enable Tx
	while(can_cmd(&message) != CAN_CMD_ACCEPTED);

	message.id.std = 0x627;
	message.dlc = 6;
	message.pt_data = Addr627;
	// --- Enable Tx
	while(can_cmd(&message) != CAN_CMD_ACCEPTED);

	message.id.std = 0x628;
	message.dlc = 6;
	message.pt_data = Addr628;
	// --- Enable Tx
	while(can_cmd(&message) != CAN_CMD_ACCEPTED);
	
	//Put CAN transceiver to sleep
	PORTF |= (1<<PF3); 
}
