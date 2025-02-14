#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <Arduino.h>

extern unsigned int msgChk(char * buffer, long length);
extern String stringPad(float value, int width, char * format);
extern float round2dec(float var);
extern long roundUp(float val);

#endif