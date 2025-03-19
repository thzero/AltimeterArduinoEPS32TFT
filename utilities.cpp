#include <Arduino.h>
#include <driver/timer.h>

#include "utilities.h"
#include "wifi.h"

void feedWatchdog() {
  // TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  // TIMERG0.wdt_feed = 1;
  // TIMERG0.wdt_wprotect = 0;
  TIMERG0.wdtwprotect.wdt_wkey = 1356348065;
  TIMERG0.wdtfeed.wdt_feed = 1;
  TIMERG0.wdtwprotect.wdt_wkey = 0;
}

unsigned int msgChk(char * buffer, long length) {
  long index;
  unsigned int checksum;

  for (index = 0L, checksum = 0; index < length; checksum += (unsigned int) buffer[index++]);
  return (unsigned int) (checksum % 256);
}

String stringPad(const char* value, int width) {
  // Serial.print(F("\nwidth: "));
  // Serial.print(width);
  // Serial.print(F(","));
  // Serial.print(F("value: "));
  // Serial.print(value);
  // Serial.print(F(","));
  int valueLength = strlen(value);
  // Serial.print(F("length: "));
  // Serial.print(valueLength);
  // Serial.print(F(","));
  valueLength = width - valueLength;
  // Serial.print(F("length2: "));
  // Serial.print(valueLength);
  // Serial.print(F(","));
  if (valueLength < 0)
    valueLength = 0;
  if (valueLength > width)
    valueLength = width;
  // Serial.print(F("length3: "));
  // Serial.println(valueLength);
  // Serial.print(F(","));
  
  char blank[valueLength + 1];
  snprintf(blank, sizeof(blank), "%*s", valueLength, ""); 
  int lengthB = snprintf(blank, valueLength, "%*s", valueLength, "");
  // Serial.print(F("lengthB: "));
  // Serial.print(lengthB);
  // Serial.print(F(","));
  // Serial.print(F("blank: '"));
  // Serial.print(blank);
  // Serial.print(F("'"));
  return blank;
}

String stringPad(float value, int width, char * format) {
  char buffer[40];
  // Serial.print(F("\nwidth: "));
  // Serial.print(width);
  // Serial.print(F(","));
  // Serial.print(F("value: "));
  // Serial.print(value);
  // Serial.print(F(","));
  // Convert float to string
  sprintf(buffer, format, value); 
  // Serial.print(F("buffer: "));
  // Serial.print(buffer);
  // Serial.print(F(","));
  int valueLength = strlen(buffer);
  // Serial.print(F("length: "));
  // Serial.print(valueLength);
  // Serial.print(F(","));
  valueLength = width - valueLength;
  // Serial.print(F("length2: "));
  // Serial.print(valueLength);
  // Serial.print(F(","));
  if (valueLength < 0)
    valueLength = 0;
  if (valueLength > width)
    valueLength = width;
  // Serial.print(F("length3: "));
  // Serial.print(valueLength);
  // Serial.print(F(","));
  
  char blank[valueLength + 1];
  int lengthB = snprintf(blank, valueLength, "%*s", valueLength, "");
  // Serial.print(F("lengthB: "));
  // Serial.print(lengthB);
  // Serial.print(F(","));
  // Serial.print(F("blank: '"));
  // Serial.print(blank);
  // Serial.print(F("'"));
  return blank;
}

String stringPad(int value, int width) {
  char buffer[40];
  // Serial.print(F("\nwidth: "));
  // Serial.print(width);
  // Serial.print(F(","));
  // Serial.print(F("value: "));
  // Serial.print(value);
  // Serial.print(F(","));
  // Convert int to string
  sprintf(buffer, "5d", value); 
  // Serial.print(F("buffer: "));
  // Serial.print(buffer);
  // Serial.print(F(","));
  int valueLength = strlen(buffer);
  // Serial.print(F("length: "));
  // Serial.print(valueLength);
  // Serial.print(F(","));
  valueLength = width - valueLength;
  // Serial.print(F("length2: "));
  // Serial.print(valueLength);
  // Serial.print(F(","));
  if (valueLength < 0)
    valueLength = 0;
  if (valueLength > width)
    valueLength = width;
  // Serial.print(F("length3: "));
  // Serial.println(valueLength);
  // Serial.print(F(","));
  
  char blank[valueLength + 1];
  int lengthB = snprintf(blank, valueLength, "%*s", valueLength, "");
  // Serial.print(F("lengthB: "));
  // Serial.print(lengthB);
  // Serial.print(F(","));
  // Serial.print(F("blank: '"));
  // Serial.print(blank);
  // Serial.print(F("'"));
  return blank;
}

float round2dec(float var) {
  // 37.66666 * 100 =3766.66
  // 3766.66 + .5 =3767.16    for rounding off value
  // then type cast to int so value is 3767
  // then divided by 100 so the value converted into 37.67
  float value = (int)(var * 100 + .5);
  return (float)value / 100;
}

long roundUp(float val) {
  long ret = (long)val;
  if (val > ret)
    return ret + 1;
  return ret;
}