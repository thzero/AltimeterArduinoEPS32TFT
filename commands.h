#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <Arduino.h>

extern void interpretCommandBuffer();
extern bool readSerial(unsigned long timestamp, unsigned long delta);

#endif