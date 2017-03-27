/**
	Standard debug utils
	
	HieuNT
	12/7/2016
**/

#include "user_debug.h"
#include <stdarg.h>
#include <stdio.h>

void user_debug_init()
{
	SERIAL_DEBUG.begin(115200);
}

static  uint8_t  buffer[DEBUG_BUFSZ];
static void uart_vprintf(const __FlashStringHelper *fmt, va_list vArgs)
{
	uint32_t len;
	len = vsprintf_P((char *)&buffer[0], (const char *)fmt, vArgs);
	uint8_t *p = buffer;
	while(*p){
		SERIAL_DEBUG.write(*p++);
	}
}

/* ------------------- MAIN DEBUG I/O -------------- */
void user_debug_print(int debugLevel, const __FlashStringHelper *module, int line, const __FlashStringHelper *fmt, ...)
{
	// __debug_printf("%d@%s: ", line, module);

	switch (debugLevel){
		case 1: 
			// "->[ERROR](module:line): "
			SERIAL_DEBUG.print(F("->[ERROR]("));
			SERIAL_DEBUG.print(module);
			SERIAL_DEBUG.print(F(":"));
			SERIAL_DEBUG.print(line,DEC);
			SERIAL_DEBUG.print(F("): "));
			break;
		case 2: 
			// "->[WARN](module:line): "
			SERIAL_DEBUG.print(F("->[WARN]("));
			SERIAL_DEBUG.print(module);
			SERIAL_DEBUG.print(F(":"));
			SERIAL_DEBUG.print(line,DEC);
			SERIAL_DEBUG.print(F("): "));
			break;
		case 3:
		case 4:
		// Don't need to print INFO, DEBUG :P
		default:
			// "->module:line:"
			SERIAL_DEBUG.print(F("->("));
			SERIAL_DEBUG.print(module);
			SERIAL_DEBUG.print(F(":"));
			SERIAL_DEBUG.print(line,DEC);
			SERIAL_DEBUG.print(F("): "));
			break;
	}

	va_list     vArgs;		    
	va_start(vArgs, fmt);
	uart_vprintf(fmt, vArgs);
	va_end(vArgs);
}

void user_debug_print_exact(const __FlashStringHelper *fmt, ...)
{
	va_list     vArgs;		    
	va_start(vArgs, fmt);
	uart_vprintf(fmt, vArgs);
	va_end(vArgs);
}
