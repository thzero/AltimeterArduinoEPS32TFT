#ifndef _TIME_H
#define _TIME_H

#include <Arduino.h>

extern unsigned long getEpoch();
extern void getTime();
extern void setTime(unsigned long time);

#endif