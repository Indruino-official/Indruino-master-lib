
/*
	Source file for Indruino Master

	HieuNT
	04/03/2017
*/

#define __DEBUG__ 4
#ifndef __MODULE__ 
#define __MODULE__ "indr_master"
#endif
#include "user_debug.h"

#include "Indruino_Master.h"

Indruino_Master::Indruino_Master(HardwareSerial& serial, Indru_Master_Callback unit_callback,
	const Indruino_Master_Unit_Management_t *configUnitTable, uint8_t totalUnit)
	:toUnitBus(serial, unit_bus_callback), _unit_callback(unit_callback), 
	_configUnitTable(configUnitTable), totalUnit(totalUnit)
{
	toUnitBus.set_parent((void *)this);
}

void Indruino_Master::begin()
{
	toUnitBus.begin();
}

void Indruino_Master::process()
{
	toUnitBus.receive();
}

void Indruino_Master::send_check_unit(uint8_t numOfUnit)
{
	toUnitBus.send_check_unit(txSeq++, numOfUnit, 0, INDRU_MASTER_BASE_ADDR);
}

static void Indruino_Master::unit_bus_callback(void *ref, Indr_Proto_t *proto, uint16_t len)
{
	Indruino_Bus *bus = (Indruino_Bus *)ref;
	Indruino_Master *master = (Indruino_Master *)(bus->parent);
	int totalUnitInfo;

	// debug("dummy tx.....................\n");
	// bus->dummy_rx_packet();

	debug("unit packet, len: %u, cmd: ", len); debug_native(indru_proto_debug_cmd_str(proto->cmd));debugx("\n");
	
	switch (proto->cmd){
		case INDR_CHECK:
			totalUnitInfo = (ARG_LEN_CALC_FROM_PROTO_LEN(len) - sizeof(proto->arg.res.check));
			if (master->totalUnit == proto->arg.res.check.totalUnit && 
				(master->totalUnit * sizeof(Indruino_Master_Unit_Management_t)) == totalUnitInfo){
				Indruino_Master_Unit_Management_t *unitInfo = (Indruino_Master_Unit_Management_t *)((uint32_t)&proto->arg.res.check + sizeof(proto->arg.res.check));
				master->unitChecked = true;
				for (uint8_t i = 0; i < master->totalUnit; ++i)
				{
					// double check order: first item is the latest unit
					if (unitInfo->unitId != (master->totalUnit - i - 1)){
						error("Order of item vs. unit\n");
						master->unitChecked = false;
						break;
					}
					// double check vs. expected unit table
					if (master->_configUnitTable[unitInfo->unitId].unitId != unitInfo->unitId ||
						master->_configUnitTable[unitInfo->unitId].unitModel != unitInfo->unitModel ||
						master->_configUnitTable[unitInfo->unitId].unitBaseAddr != unitInfo->unitBaseAddr){
						error("unexpected value vs. config\n");
						debug("unitId: %u, %u\n", master->_configUnitTable[unitInfo->unitId].unitId, unitInfo->unitId);
						debug("unitModel: %u, %u\n", master->_configUnitTable[unitInfo->unitId].unitModel, unitInfo->unitModel);
						debug("unitBaseAddr: %u, %u\n", master->_configUnitTable[unitInfo->unitId].unitBaseAddr, unitInfo->unitBaseAddr);
						master->unitChecked = false;
						break;
					}
					unitInfo = (uint32_t)unitInfo + sizeof(Indruino_Master_Unit_Management_t);	
				}
				if (master->unitChecked){
					debug("Unit table check passed: \n");
					for (uint8_t i = 0; i < master->totalUnit; ++i)
					{
						debug("[%u] id: %u, model: ", i, master->_configUnitTable[i].unitId);
						debug_native(indru_unit_profile_debug_model_str(master->_configUnitTable[i].unitModel));
						debugx(", baseAddr: 0x%08lX\n", master->_configUnitTable[i].unitBaseAddr);
					}	
				}
			}
			else {
				if (master->totalUnit != proto->arg.res.check.totalUnit)
					error("Total unit: expected %u, actual: %u\n", master->totalUnit, proto->arg.res.check.totalUnit);
				if ((master->totalUnit * sizeof(Indruino_Master_Unit_Management_t)) != totalUnitInfo)
					error("Total unit (len calc): expected: %u, actual: %u\n", 
						(master->totalUnit * sizeof(Indruino_Master_Unit_Management_t)), totalUnitInfo);
			}
			break;
		case INDR_STA:

			break;
		case INDR_RESET:

			break;
		case INDR_WRITE:

			break;
		case INDR_READ:

			break;
	}

	// call app callback
	if (master->_unit_callback != NULL) master->_unit_callback(proto, len);
}
