#include <Arduino.h>

#include "leds.h"

ledsBuiltin::ledsBuiltin() {
}

void ledsBuiltin::setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void ledsBuiltin::turnOff() {
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println(F("LED is off"));
}

void ledsBuiltin::turnOn() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println(F("LED is on"));
}

ledsBuiltin _ledsBuiltin;
