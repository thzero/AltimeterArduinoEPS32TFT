#include "leds.h"

void setupLedBuiltin() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void turnOffLedBuiltin() {
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println(F("LED is off"));
}

void turnOnLedBuiltin() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println(F("LED is on"));
}