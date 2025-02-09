#include "debug.h"

///////
/// char[]
///////
void debug(char str[]) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.println(str);
#endif
}

void debug(char str[], char value[]) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(char str[], double value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(char str[], float value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(char str[], int value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(char str[], long value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(char str[], String value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(char str[], unsigned int value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(char str[], unsigned long value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

///////
/// F macro
///////
void debug(const __FlashStringHelper *ifsh) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.println(reinterpret_cast<const char *>(ifsh));
#endif
}

void debug(const __FlashStringHelper *ifsh, char value[]) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(reinterpret_cast<const char *>(ifsh));
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(const __FlashStringHelper *ifsh, double value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(reinterpret_cast<const char *>(ifsh));
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(const __FlashStringHelper *ifsh, float value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(reinterpret_cast<const char *>(ifsh));
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(const __FlashStringHelper *ifsh, int value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(reinterpret_cast<const char *>(ifsh));
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(const __FlashStringHelper *ifsh, long value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(reinterpret_cast<const char *>(ifsh));
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(const __FlashStringHelper *ifsh, String value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(reinterpret_cast<const char *>(ifsh));
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(const __FlashStringHelper *ifsh, unsigned int value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(reinterpret_cast<const char *>(ifsh));
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(const __FlashStringHelper *ifsh, unsigned long value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(reinterpret_cast<const char *>(ifsh));
  Serial.print(F("="));
  Serial.println(value);
#endif
}

///////
/// STRING
///////
void debug(String str) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.println(str);
#endif
}

void debug(String str, double value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(String str, float value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(String str, int value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(String str, long value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(String str, String value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(String str, unsigned int value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}

void debug(String str, unsigned long value) {
#if defined(DEBUG) || defined(DEBUG_INTERNAL)
  Serial.print(str);
  Serial.print(F("="));
  Serial.println(value);
#endif
}