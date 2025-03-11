#include "time.h"
#include <TimeLib.h>

#include <ESP32Time.h>

ESP32Time rtc;

char* convertTime(unsigned long epochS) {
    char buff[32];
    sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", month(epochS), day(epochS), year(epochS), hour(epochS), minute(epochS), second(epochS));
    return buff;
}

unsigned long getEpoch() {
    return rtc.getEpoch();
}

void getTime() {
    Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));
}

void setTime(unsigned long time) {
    rtc.setTime(time);
}