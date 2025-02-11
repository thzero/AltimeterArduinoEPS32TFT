#include <Arduino.h>

#include "leds.h"

leds::leds() {
}

void leds::setupBuiltin() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void leds::turnOffBuiltin() {
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println(F("LED is off"));
}

void leds::turnOnBuiltin() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println(F("LED is on"));
}

leds _leds;
