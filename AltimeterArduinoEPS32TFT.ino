#include <Arduino.h>
#include <esp_sleep.h>
#include <Preferences.h>
#include <Wire.h>

// #include "ble.h"
#include "button.h"
#include "constants.h"
#include "debug.h"
#include "flightLogger.h"
#include "leds.h"
#include "loopThrottle.h"
#include "memory.h"
#include "neoPixel.h"
#include "stateMachine.h"
#include "sensor.h"
#include "tft.h"
#include "utilities.h"
#include "wifi.h"

unsigned long _timestamp;

/*
  This will turn off the altimeter
*/
void sleepDevice() {
  setupButtonDeninit();
  
  _ledsBuiltin.off();

  _neoPixelBlinker.setupDeinit();

  drawTftReset();
  drawTftSleep();

  sleepSensors();

  sleepTft();

  _neoPixel.sleep();

  pinMode(41, OUTPUT);
  pinMode(42, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(7, OUTPUT);

  delay(100);
  digitalWrite(4, LOW);

  // SDA SDL pins to the Stemma QT need to be set high as they have physical pull up resistors (2x10K)
  // PS Ram CS and TFT CS should be high.
  digitalWrite(41, HIGH);
  digitalWrite(42, HIGH);
  digitalWrite(26, HIGH);
  digitalWrite(7, HIGH);
  delay(10);

  sleepTft();

  gpio_deep_sleep_hold_en();

  sleepTftHold();
  
  _neoPixel.sleepHold();
  
  gpio_hold_en((gpio_num_t)41);
  gpio_hold_en((gpio_num_t)42);
  gpio_hold_en((gpio_num_t)26);
  gpio_hold_en((gpio_num_t)7);
  delay(2000);

  esp_deep_sleep_start();
}

loopThrottle _throttleMemory;
void loop() {
  // put your main code here, to run repeatedly

  unsigned long current = millis();
  unsigned long delta = current - _timestamp;

  // Determine the memory loop time delay based on sampling rate.
  int deltaMemory = _throttleMemory.determine(delta, (int)SAMPLE_RATE_MEMORY);
  if (deltaMemory > 0) {
    _memory.loop();
    // Serial.print(F("_throttleMemory..."));
    // Serial.println(deltaMemory);
  }

  _stateMachine.loop(current, delta);

  _timestamp = current;
}

void setupFlightLogger() {
  Serial.println(F("Setup flight logger..."));

  Serial.println(F("Setup file system..."));

  if (!_flightLogger.instance.initFileSystem()) {
    Serial.println(F("Failed to initialize file system"));
    return;
  }

  Serial.println(F("...file system successful."));

  Serial.println(F("Initialize flight logger..."));

  _flightLogger.instance.initFlight();

  Serial.println(F("...flight logger successful."));
}

void setupFlightLoggerInitialAtmosphere() {
  Serial.println(F("Setup initial atmosphere..."));
  
  atmosphereValues values = initializeSensorsAtmosphere();
  _flightLogger.data.altitudeInitial = values.altitude;
  Serial.print(F("initial altitude: "));
  Serial.println(_flightLogger.data.altitudeInitial);

  _flightLogger.data.pressureInitial = values.pressure;
  Serial.print(F("initial pressure: "));
  Serial.println(_flightLogger.data.pressureInitial);

  _flightLogger.data.temperatureInitial = values.temperature;
  Serial.print(F("initial temperature: "));
  Serial.println(_flightLogger.data.temperatureInitial);

  Serial.println(F("...initial atmosphere initialized."));
}

// void setupNetwork() {
//   _ble.start();
//   _wifi.start();
// }

void setup() {
  // put your setup code here, to run once:
  Serial.println(F("Setup..."));
  Serial.println(F(""));

  _ledsBuiltin.setup();
  _neoPixelBlinker.setup();

  _ledsBuiltin.on();

  Wire.begin();
  Serial.begin(SERIAL_BAUD);

  setupFlightLogger();

  setupTft();

  setupSensors();

  setupFlightLoggerInitialAtmosphere();

  setupSensorsCompleted();

  setupButton();

  // Setup WiFi so it's available...
  _wifi.start();

  // Disable Network
  setupNetworkDisable();

  // battery
  /*
   .. Setup 1:1 ratio voltage divider from VBAT -> Pin 10 -> Pin 6
    analogReadResolution(12);
    pinMode(10,INPUT);   
    pinMode(6, OUTPUT);
    digitalWrite(6, LOW);
  */

  drawTftSplash();

  Serial.println(F(""));
  Serial.print(BOARD_FIRMWARE);

  Serial.print(F(", version: "));
  Serial.print(MAJOR_VERSION);
  Serial.print(F("."));
  Serial.print(MINOR_VERSION);
#if defined(DEV) || defined(DEV_SIM)
  Serial.println(F(" (DEV)"));
#endif
  Serial.println(F(""));

  _memory.setup();

  Serial.println(F("Memory Available"));
  Serial.print(F("\tFree Heap"));
  Serial.print(F("\tFree Internal Heap"));
  Serial.println(F("\tFree Minimum Heap"));
  Serial.print(F("\t"));
  Serial.print(_memory.heap());
  Serial.print(F("kb\t\t"));
  Serial.print(_memory.heapInternal());
  Serial.print(F("kb\t\t\t"));
  Serial.print(_memory.heapMinimum());
  Serial.println(F("kb"));

  _ledsBuiltin.off();

  Serial.println(F(""));
  Serial.println(F("...finished."));
  Serial.println(F(""));
}