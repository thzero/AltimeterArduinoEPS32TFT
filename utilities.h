#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <Arduino.h>

extern void feedWatchdog();
extern unsigned int msgChk(char * buffer, long length);
extern String stringPad(const char* value, int width);
extern String stringPad(float value, int width, char * format);
extern String stringPad(int value, int width);
extern float round2dec(float var);
extern long roundUp(float val);

#endif