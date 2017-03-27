/*
  // HieuNT: add debug to example below
  // 4/3/2017

  FastCRC-Example

  (c) Frank Boesing 2014
*/

#define __DEBUG__ 4
#ifndef __MODULE__ 
#define __MODULE__ "Test"
#endif
#include "user_debug.h"

#include "Indruino_Protocol.h"

#include <FastCRC.h>

FastCRC16 CRC16;

uint8_t buf[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

void setup() {

  // delay(1500);
  user_debug_init();
  debug("\nHello Indruino\n");

  debug("CRC Example\n");

  debug("CCITT-CRC of \"");

  for (unsigned int i = 0; i < sizeof(buf); i++) {
    debugx("%c ", (char) buf[i]);
  }
  debugx("\n");

  uint16_t crc16 = CRC16.ccitt(buf, sizeof(buf));
  debug("\" is: 0x%04X\n", crc16);

  // Serial.print(crc16, HEX);
}


void loop() {
}


