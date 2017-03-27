// HieuNT: add debug to example below
// 4/3/2017

// Demo Code for SerialCommand Library
// Steven Cogswell
// May 2011

#define __DEBUG__ 4
#ifndef __MODULE__ 
#define __MODULE__ "Test"
#endif
#include "user_debug.h"

#include "Indruino_Protocol.h"

#include <SerialCommand.h>

#define arduinoLED 13   // Arduino LED on board

SerialCommand sCmd;     // The demo SerialCommand object

void setup() {
  pinMode(arduinoLED, OUTPUT);      // Configure the onboard LED for output
  digitalWrite(arduinoLED, LOW);    // default to LED off

  user_debug_init();
  debug("\nHello Indruino\n");

  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("ON",    LED_on);          // Turns LED on
  sCmd.addCommand("OFF",   LED_off);         // Turns LED off
  sCmd.addCommand("HELLO", sayHello);        // Echos the string argument back
  sCmd.addCommand("P",     processCommand);  // Converts two arguments to integers and echos them back
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  debug("Ready\n");
}

void loop() {
  sCmd.readSerial();     // We don't do much, just process serial commands
}


void LED_on() {
  debug("LED on\n");
  digitalWrite(arduinoLED, HIGH);
}

void LED_off() {
  debug("LED off\n");
  digitalWrite(arduinoLED, LOW);
}

void sayHello() {
  char *arg;
  arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {    // As long as it existed, take it
    debug("Hello %s\n", arg);
  }
  else {
    debug("Hello, whoever you are\n");
  }
}

void processCommand() {
  int aNumber;
  unsigned long longNum;

  char *arg;

  debug("We're in processCommand\n");
  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    debug("First argument was: %d\n", aNumber);
  }
  else {
    debug("No arguments\n");
  }

  arg = sCmd.next();
  if (arg != NULL) {
    longNum = atol(arg);
    debug("Second argument was: %lu\n", longNum);
  }
  else {
    debug("No second argument\n");
  }
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  debug("What?\n");
}
