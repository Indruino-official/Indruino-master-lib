
/*
	Source file for Indruino Unit Profile

	HieuNT
	05/03/2017
*/

#include "Indruino_Unit_Profile.h"


// Debug
const __FlashStringHelper *indru_unit_profile_debug_model_str(uint16_t model)
{
	switch (model){
		case ID_C32R_MODEL:
			return F("ID_C32R_MODEL");
		case ID_C32X_MODEL:
			return F("ID_C32X_MODEL");
	}
	return F("NOT FOUND!");
}
