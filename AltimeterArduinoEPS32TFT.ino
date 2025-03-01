//
// https://www.aliexpress.us/item/3256806269616675.html?spm=a2g0o.order_list.order_list_main.5.16c41802aIJMaX&gatewayAdapt=vnm2usa4itemAdapt
//

#include <Arduino.h>
#include <esp_sleep.h>
#include <Preferences.h>
#include <Wire.h>

// #include "ble.h"
#include "button.h"
#include "constants.h"
#include "debug.h"
#include "fileSystem.h"
#include "flightLogger.h"
#include "leds.h"
#include "loopThrottle.h"
#include "monitor.h"
#include "neoPixel.h"
#include "network.h"
#include "stateMachine.h"
#include "sensor.h"
#include "tft.h"
#include "utilities.h"

unsigned long _timestamp;

static const char* TAG = "AltimeterArduinoESP32TFT";

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

loopThrottle _throttleMnitor;
void loop() {
  // put your main code here, to run repeatedly

  unsigned long current = millis();
  unsigned long delta = current - _timestamp;

  // Determine the monitor loop time delay based on sampling rate.
  int deltaMonitor = _throttleMnitor.determine(delta, (int)SAMPLE_RATE_MONITOR);
  if (deltaMonitor > 0) {
    _monitor.loop();
    // Serial.print(F("_throttleMnitor..."));
    // Serial.println(deltaMonitor);
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

  Serial.println(F("...flight logger successful."));
}

void setupInitialAtmosphere() {
  Serial.println(F("Setup initial atmosphere..."));
  
  atmosphereValues values = initializeSensorsAtmosphere();
  _flightLogger.altitudeInitial = values.altitude;
  Serial.print(F("initial altitude: "));
  Serial.println(_flightLogger.altitudeInitial);

  _flightLogger.pressureInitial = values.pressure;
  Serial.print(F("initial pressure: "));
  Serial.println(_flightLogger.pressureInitial);

  _flightLogger.temperatureInitial = values.temperature;
  Serial.print(F("initial temperature: "));
  Serial.println(_flightLogger.temperatureInitial);

  Serial.println(F("...initial atmosphere initialized."));
}

void setup() {
  // Enables the ESP_LOGX to go to Serial output.
#ifdef DEBUG
  Serial.setDebugOutput(true);
#endif

  // put your setup code here, to run once:
  Serial.println(F("Setup..."));
  Serial.println(F(""));

  _ledsBuiltin.setup();
  _neoPixelBlinker.setup();

  _ledsBuiltin.on();

  Wire.begin();
  Serial.begin(SERIAL_BAUD);

  setupFileSystem();

  setupFlightLogger();

  setupTft();

  setupSensors();

  setupInitialAtmosphere();

  setupSensorsCompleted();

  setupButton();

  // Setup Network so it's available...
  setupNetwork();
  networkStart(); // TODO: temp

  drawTftSplash();

  _stateMachine.setup();

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

  _monitor.setup();

#ifdef MONITOR_MEMORY
  Serial.println(F("Memory Available"));
  Serial.print(F("\tFree Heap"));
  Serial.print(F("\tFree Internal Heap"));
  Serial.println(F("\tFree Minimum Heap"));
  Serial.print(F("\t"));
  Serial.print(_monitor.heap());
  Serial.print(F("kb\t\t"));
  Serial.print(_monitor.heapInternal());
  Serial.print(F("kb\t\t\t"));
  Serial.print(_monitor.heapMinimum());
  Serial.println(F("kb"));
#endif

#ifdef MONITOR_BATTERY
  Serial.println(F("Voltage"));
  Serial.print(_monitor.voltage());
  Serial.println(F("V"));
#endif

  _ledsBuiltin.off();

  Serial.println(F(""));
  Serial.println(F("...finished."));
  Serial.println(F(""));
}