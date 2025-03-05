#include "time.h"

#include <ESP32Time.h>

ESP32Time rtc;

unsigned long getEpoch() {
    return rtc.getEpoch();
}

void getTime() {
    Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));
}

void setTime(unsigned long time) {
    rtc.setTime(time);
}