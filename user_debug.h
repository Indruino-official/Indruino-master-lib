/**
	Standard debug utils
	
	HieuNT
	12/7/2016
**/


#ifndef _USER_DEBUG_
#define _USER_DEBUG_

#include <Arduino.h>

// Use Serial 1 for debug
#define SERIAL_DEBUG	Serial
#define DEBUG_BUFSZ		256

#ifdef __cplusplus
extern "C" {
#endif

void user_debug_init(void);
// HieuNT note: `module` and `fmt` should be used with F() or PSTR() to save RAM
void user_debug_print(int debugLevel, const __FlashStringHelper *module, int line, const __FlashStringHelper *fmt, ...);
void user_debug_print_exact(const __FlashStringHelper *fmt, ...);

#if __DEBUG__ > 0
#ifndef __MODULE__
#error "__MODULE__ must be defined"
#endif
#endif

#if __DEBUG__ >= 1
#define error(x,...) do{ user_debug_print(1, F(__MODULE__), __LINE__, F(x), ##__VA_ARGS__); }while(0)
#else
#define error(...) 
#endif

#if __DEBUG__ >= 2
#define warn(x,...) do{ user_debug_print(2, F(__MODULE__), __LINE__, F(x), ##__VA_ARGS__); }while(0)
#else
#define warn(...) 
#endif

#if __DEBUG__ >= 3
#define info(x,...) do{ user_debug_print(3, F(__MODULE__), __LINE__, F(x), ##__VA_ARGS__); }while(0)
#else
#define info(...) 
#endif

#if __DEBUG__ >= 4
#define debug(x,...) do{ user_debug_print(4, F(__MODULE__), __LINE__, F(x), ##__VA_ARGS__); }while(0)
#define debugx(x,...) do{ user_debug_print_exact(F(x), ##__VA_ARGS__); }while(0)
#define debug_native(...) SERIAL_DEBUG.print(__VA_ARGS__)
#else
#define debug(...) 
#define debugx(...) 
#define debug_native(...)
#endif


#ifdef __cplusplus
}
#endif
 
#endif /* _USER_DEBUG_ */




