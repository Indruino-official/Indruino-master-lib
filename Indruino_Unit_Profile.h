/*
	Header file for Indruino Unit Model Profile

	HieuNT
	05/03/2017
*/

#ifndef __INDRUINO_UNIT_PROFILE_
#define __INDRUINO_UNIT_PROFILE_

#include <Arduino.h>

// All Indruino unit models

// ID_C32R
#define ID_C32R_MODEL 		0
#define ID_C32R_ADDR_RANGE 	1024

// ID_C32X
#define ID_C32X_MODEL 		1
#define ID_C32X_ADDR_RANGE 	1024

// For generic digital input/output model
typedef struct {
	uint8_t ioDefNum;
	// arduino pin num
	uint8_t pinNum; 
} DigitalIO_Mapping_Table_t;

// debug
const __FlashStringHelper *indru_unit_profile_debug_model_str(uint16_t model);

#endif
