#ifndef _DEBUG_H
#define _DEBUG_H

#include <Arduino.h>

// #define DEBUG
#define DEBUG_INTERNAL
// #define DEBUG_SENSORS

///////
/// char[]
///////
extern void debug(char str[]);
extern void debug(char str[], char value[]);
extern void debug(char str[], double value);
extern void debug(char str[], float value);
extern void debug(char str[], long value);
extern void debug(char str[], int value);
extern void debug(char str[], String value);
extern void debug(char str[], unsigned long value);
extern void debug(char str[], unsigned int value);

///////
/// F macro for constant string literals used serial output.
///////
extern void debug(const __FlashStringHelper *ifsh);
extern void debug(const __FlashStringHelper *ifsh, char value[]);
extern void debug(const __FlashStringHelper *ifsh, double value);
extern void debug(const __FlashStringHelper *ifsh, float value);
extern void debug(const __FlashStringHelper *ifsh, int value);
extern void debug(const __FlashStringHelper *ifsh, long value);
extern void debug(const __FlashStringHelper *ifsh, String value);
extern void debug(const __FlashStringHelper *ifsh, unsigned int value);
extern void debug(const __FlashStringHelper *ifsh, unsigned long value);

///////
/// STRING
///////
extern void debug(String str);
extern void debug(String str, char value[]);
extern void debug(String str, double value);
extern void debug(String str, float value);
extern void debug(String str, int value);
extern void debug(String str, long value);
extern void debug(String str, String value);
extern void debug(String str, unsigned int value);
extern void debug(String str, unsigned long value);

#endif