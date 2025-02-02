// SPDX-FileCopyrightText: 2023 Carter Nelson for Adafruit Industries
//
// SPDX-License-Identifier: MIT
// --------------------------------------
// i2c_scanner
//
// Modified from https://playground.arduino.cc/Main/I2cScanner/
// --------------------------------------
// https://i2cdevices.org/devices

#include <Arduino.h>
#include <Wire.h>

// Set I2C bus to use: Wire, Wire1, etc.
#define WIRE Wire

void i2CScanner() {
  byte error, address;
  int devices;

  Serial.println(F("Scanning..."));

  devices = 0;
  for (address = 1; address < 127; address++)
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    WIRE.beginTransmission(address);
    error = WIRE.endTransmission();

    if (error == 0)
    {
      Serial.print(F("I2C device found at address 0x"));
      if (address < 16)
        Serial.print(F("0"));
      Serial.print(address,HEX);
      Serial.println(F("!"));

      devices++;
    }
    else if (error == 4)
    {
      Serial.print(F("Unknown error at address 0x"));
      if (address < 16)
        Serial.print(F("0"));
      Serial.println(address,HEX);
    }
  }
  
  if (devices == 0) {
    Serial.println(F("No I2C devices found."));
    return;
  }

  Serial.println(F("...scanning done!"));
}
