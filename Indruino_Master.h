/*
	Header file for Indruino Master

	HieuNT
	04/03/2017
*/

#ifndef __INDRUINO_MASTER_H_
#define __INDRUINO_MASTER_H_

#include "Indruino_Protocol.h"
#include "FastCRC.h"
#include "Indruino_Unit_Profile.h"

typedef void (*Indru_Master_Callback)(Indr_Proto_t *proto, uint16_t len);

#define INDRU_MASTER_BASE_ADDR	0x8000

// reuse :)
typedef Indr_Proto_Arg_Res_Check_Unit_Item_t Indruino_Master_Unit_Management_t;

class Indruino_Master {

public:
	Indruino_Master(HardwareSerial& serial, Indru_Master_Callback unit_callback,
		const Indruino_Master_Unit_Management_t *configUnitTable, uint8_t totalUnit);
	// begin bus serial
	void begin();
	
	// Send check unit
	void send_check_unit(uint8_t numOfUnit);

	// Main receive progress, must be periodically call in main loop
	void process();

	// Master have only one serial to communicate with all unit
	// (diff. from unit that having two!)
	Indruino_Bus toUnitBus;
	// tx sequence number (from master only)
	uint16_t txSeq;
	// application callback
	Indru_Master_Callback _unit_callback;
	// bus callback 
	static void unit_bus_callback(void *ref, Indr_Proto_t *proto, uint16_t len);
	// all unit is checked?
	boolean unitChecked = false;

	uint8_t totalUnit;
	const Indruino_Master_Unit_Management_t *_configUnitTable;

	// TODO: private, protected classify!
};

#endif
