#include "utilities.h"
#include "wifi.h"

unsigned int msgChk(char * buffer, long length) {
  long index;
  unsigned int checksum;

  for (index = 0L, checksum = 0; index < length; checksum += (unsigned int) buffer[index++]);
  return (unsigned int) (checksum % 256);
}

String stringPad(float value, int width, char * format) {
  char buffer[40];
  // Serial.print("width: ");
  // Serial.print(width);
  // Convert float to string
  sprintf(buffer, format, value); 
  // Serial.print("\tvalue: ");
  // Serial.print(value);
  int valueLength = strlen(buffer);
  // Serial.print("\tlength: ");
  // Serial.print(valueLength);
  valueLength = width - valueLength;
  // Serial.print("\tlength2: ");
  // Serial.print(valueLength);
  if (valueLength < 0)
    valueLength = 0;
  if (valueLength > width)
    valueLength = width;
  // Serial.print("\tlength3: ");
  // Serial.println(valueLength);
  
  char blank[valueLength + 1];
  snprintf(blank, sizeof(blank), "%*s", valueLength, ""); 
  // Serial.print("blank: ");
  // Serial.println(blank);
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