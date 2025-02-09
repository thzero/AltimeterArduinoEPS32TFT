#include "utilities.h"

unsigned int msgChk(char * buffer, long length) {
  long index;
  unsigned int checksum;

  for (index = 0L, checksum = 0; index < length; checksum += (unsigned int) buffer[index++]);
  return (unsigned int) (checksum % 256);
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