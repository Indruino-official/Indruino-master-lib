
/*
	Indruino Master demo

	HieuNT
	04/03/2017
*/

#define __DEBUG__ 4
#ifndef __MODULE__ 
#define __MODULE__ "demo_master"
#endif
#include "user_debug.h"

#include "Indruino_Master.h"
#include <SerialCommand.h>

#define TOTAL_UNIT	4

void master_callback(Indr_Proto_t *proto, uint16_t len);

// config unit table
const Indruino_Master_Unit_Management_t configUnitTable[TOTAL_UNIT] = {
	{.unitId = 0, .unitModel = ID_C32R_MODEL, .unitBaseAddr = INDRU_MASTER_BASE_ADDR},
	{.unitId = 1, .unitModel = ID_C32R_MODEL, .unitBaseAddr = (INDRU_MASTER_BASE_ADDR + ID_C32R_ADDR_RANGE)},
	{.unitId = 2, .unitModel = ID_C32R_MODEL, .unitBaseAddr = (INDRU_MASTER_BASE_ADDR + 2*ID_C32R_ADDR_RANGE)},
	{.unitId = 3, .unitModel = ID_C32R_MODEL, .unitBaseAddr = (INDRU_MASTER_BASE_ADDR + 3*ID_C32R_ADDR_RANGE)}
};

Indruino_Master master(Serial1, master_callback, configUnitTable, TOTAL_UNIT);

void master_callback(Indr_Proto_t *proto, uint16_t len)
{
	debug("application callback...\n");
}

void setup()
{
	user_debug_init();
	debug("\nHello Indruino Master demo\n");

	debug("Init master...");
	master.begin();
	debug("done!\n");
	
}

void loop()
{
	static uint32_t t = 0;
	if (millis() - t > 2000){
		t = millis();
		master.send_check_unit(TOTAL_UNIT);
	}

	master.process();
}
