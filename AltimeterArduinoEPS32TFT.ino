#include <Arduino.h>
#include <esp_sleep.h>
#include <Preferences.h>
#include <Wire.h>

// #include "ble.h"
#include "button.h"
#include "commands.h"
#include "constants.h"
#include "debug.h"
#include "flightLogger.h"
#include "flightLoggerBase.h"
#include "flightLoggerData.h"
#include "flightLoggerLFS.h"
#include "kalman.h"
#include "leds.h"
#include "loopThrottle.h"
#include "neoPixel.h"
#include "sensor.h"
#include "tft.h"
#include "utilities.h"
#include "wifi.h"

// Assumed environmental values
float altitudeBarometer = 1650.3;  // meters ... map readings + barometer position
byte altitudeOffsetLiftoff = 20;
int altitudeOffsetGround = altitudeOffsetLiftoff / 2;
int timeoutRecording = 300 * 10000;
int timeOutTimeToApogee = 20000;

//////////////////////////////////////////////////////////////////////
// Global variables
//////////////////////////////////////////////////////////////////////
enum loopStates {
  AIRBORNE_ASCENT,
  AIRBORNE_DESCENT,
  GROUND
};

struct loopStateMachine {
  enum loopStates current = GROUND;
} _loopState;

unsigned long _timestamp;

/*
  This will turn off the altimeter
*/
void sleepDevice() {
  setupButtonDeninit();
  setupNeoPixelBlinkerDeninit();

  drawTftReset();
  drawTftSleep();

  sleepSensors();

  sleepTft();

  sleepNeoPixel();

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
  
  sleepNeoPixelHold();
  
  gpio_hold_en((gpio_num_t)41);
  gpio_hold_en((gpio_num_t)42);
  gpio_hold_en((gpio_num_t)26);
  gpio_hold_en((gpio_num_t)7);
  delay(2000);

  esp_deep_sleep_start();
}

void loopStateAIRBORNE_ASCENTToAIRBORNE_DESCENT() {
  _flightLogger.data.altitudeApogee = _flightLogger.data.altitudeApogeeFirstMeasure;
  _flightLogger.data.timestampApogee = _flightLogger.data.timestampApogeeFirstMeasure;

  debug("");
  debug("");
  debug("");
  debug("");
  debug("...DESCENT!!!!");
  debug("...DESCENT!!!!");
  debug("...DESCENT!!!!");
  debug("");
  debug("");
  debug("");
  debug("");

  _loopState.current = AIRBORNE_DESCENT;  // TODO!
}

float loopStateAIRBORNE(unsigned long currentTimestamp, long diffTime) {
  atmosphereValues atmosphere = readSensorAtmosphere();
  float altitude = readSensorAltitude(atmosphere);
  float altitudeDelta = altitude - _flightLogger.data.altitudeLast;
  _flightLogger.data.altitudeCurrent = altitude;
#ifdef DEBUG_ALTIMETER
  debug("loopStateAIRBORNE...altitude", altitude);
  debug("loopStateAIRBORNE...altitudeDelta", altitudeDelta);
  debug("loopStateAIRBORNE...altitudeCurrent", _flightLogger.data.altitudeCurrent);
#endif
  _flightLogger.data.timestampCurrent = currentTimestamp;

  // Log the flight altitude...
  _flightLogger.instance.setFlightTime(diffTime);
  _flightLogger.instance.setFlightAltitude(_flightLogger.data.altitudeCurrent);

  // Log the flight temperature and pressure...
  atmosphereValues val = readSensorAtmosphere();
  _flightLogger.instance.setFlightHumidity((long)val.temperature);
  _flightLogger.instance.setFlightTemperature((long)val.temperature);
  _flightLogger.instance.setFlightPressure((long)val.pressure);

  // Log the flight x, y, and z accelerations...
  // if (_qmi.instance.getDataReady()) {
  //   if (_qmi.instance.getAccelerometer(_qmi.acc.x, _qmi.acc.y, _qmi.acc.z)) {
  //     _flightLogger.instance.setFlightAccelX(_qmi.acc.x);
  //     _flightLogger.instance.setFlightAccelY(_qmi.acc.y);
  //     _flightLogger.instance.setFlightAccelZ(_qmi.acc.z);
  //   }
  // }

  // Add to the set data to the flight.
  _flightLogger.instance.setFlightData();

  _flightLogger.data.timestampPrevious = currentTimestamp;
  _flightLogger.data.altitudeLast = altitude;

  // sendTelemetry(timestamp - _flightLogger.data.timestampInitial , atmosphere, accelerometer, gryoscope, altitude, diffTime);

  drawTftFlightAirborne(currentTimestamp, diffTime);

  return altitudeDelta;
}

loopThrottle _throttleAirborneAscent;
void loopStateAIRBORNE_ASCENT(unsigned long timestamp, unsigned long deltaElapsed) {
  int delta = _throttleAirborneAscent.determine(deltaElapsed, (int)SAMPLE_RATE_AIRBORNE_ASCENT);
  if (delta == 0)
    return;

  long currentTimestamp = timestamp - _flightLogger.data.timestampLaunch;
#ifdef DEBUG_ALTIMETER
  debug("loopStateAIRBORNE_ASCENT...timestamp", timestamp);
  debug("loopStateAIRBORNE_ASCENT...currentTimestamp", currentTimestamp);
#endif

  // Determine different in time between the last step...
  long diffTime = currentTimestamp - _flightLogger.data.timestampPrevious;

  float altitudeDelta = loopStateAIRBORNE(currentTimestamp, diffTime);

  // Detect apogee by taking X number of measures as long as the current is less
  // than the last recorded altitude.
#ifdef DEBUG_ALTIMETER
  debug("loopStateAIRBORNE_ASCENT...altitudeLast", _flightLogger.data.altitudeLast);
  debug("loopStateAIRBORNE_ASCENT...altitudeDelta", altitudeDelta);
#endif
  if (altitudeDelta < 0) {
    if (_flightLogger.data.measures == SAMPLE_MEASURES_APOGEE) {
      // Detected apogee.
      _flightLogger.data.altitudeApogeeFirstMeasure = _flightLogger.data.altitudeLast;
      _flightLogger.data.timestampApogeeFirstMeasure = currentTimestamp;
    }
    _flightLogger.data.measures = _flightLogger.data.measures - 1;
#ifdef DEBUG_ALTIMETER
    debug("loopStateAIRBORNE_ASCENT...measures", _flightLogger.data.measures);
#endif
    if (_flightLogger.data.measures == 0) {
      // Detected apogee.
      loopStateAIRBORNE_ASCENTToAIRBORNE_DESCENT();
      return;
    }
  } 
  else {
    if (_flightLogger.data.measures < SAMPLE_MEASURES_APOGEE) {
#ifdef DEBUG_ALTIMETER
      debug("loopStateAIRBORNE_ASCENT...measures", "reset");
#endif
      // If the curent is greater than the last, then reset as it was potentially
      // a false positive.
      _flightLogger.data.altitudeApogeeFirstMeasure = 0;
      _flightLogger.data.timestampApogeeFirstMeasure = 0;
      _flightLogger.data.altitudeLast = _flightLogger.data.altitudeCurrent;
      _flightLogger.data.measures = SAMPLE_MEASURES_APOGEE;
    }
  }

  // Check for timeouts...
  bool timestampApogeeCheck = currentTimestamp > timeOutTimeToApogee;
#ifdef DEBUG_ALTIMETER
  debug("loopStateAIRBORNE_ASCENT...timeOutTimeToApogee", timeOutTimeToApogee);
  debug("loopStateAIRBORNE_ASCENT...currentTimestamp", currentTimestamp);
  debug("loopStateAIRBORNE_ASCENT...timestampApogeeCheck", timestampApogeeCheck);
#endif
  if (timestampApogeeCheck) {
    // Something went wrong and apogee was never found, so abort!
    loopStateAIRBORNEToAbort("Time to apogee threshold exceeded!", "AIRBORNE_ASCENT aborted, returning to GROUND!!!!");
    return;
  }

  bool timeoutRecordingCheck = currentTimestamp > timeoutRecording;
#ifdef DEBUG_ALTIMETER
  debug("loopStateAIRBORNE_ASCENT...timestampLaunch", _flightLogger.data.timestampLaunch);
  debug("loopStateAIRBORNE_ASCENT...timeoutRecording", timeoutRecording);
  debug("loopStateAIRBORNE_ASCENT...currentTimestamp", currentTimestamp);
  debug("loopStateAIRBORNE_ASCENT...timeoutRecordingCheck", timeoutRecordingCheck);
#endif
  if (timeoutRecordingCheck) {
    // Something went wrong., so abort!
    loopStateAIRBORNEToAbort("Time to apogee threshold exceeded!", "AIRBORNE_ASCENT aborted, returning to GROUND!!!!");
    return;
  }
}

void loopStateAIRBORNEToAbort(char message1[], char message2[]) {
  // Something went wrong and aborting...

  // _loopState.current = GROUND; // TODO
  _flightLogger.data.airborne = false;
  _flightLogger.aborted = true;
  _flightLogger.recording = false;
  _flightLogger.data.touchdown = true;

  debug("");
  debug("");
  debug("");
  debug("");
  debug("...ABORTED!!!!");
  debug("...ABORTED!!!!");
  Serial.println(message1);
  Serial.println(message2);
  debug("...ABORTED!!!!");
  debug("");
  debug("");
  debug("");
  debug("");

  // To avoid battery drain, etc. user should choose to turn on networking...
  // setupNetwork();

  drawTftReset();
  drawTftSplash();
}

void loopStateAIRBORNEToGROUND() {
  // Complete the flight
  Serial.println("Flight has ended!!!");

  _loopState.current = GROUND; // TODO
  _flightLogger.data.airborne = false;
  _flightLogger.recording = false;

  // To avoid battery drain, etc. user should choose to turn on networking...
  // setupNetwork();

  debug("");
  debug("");
  debug("");
  debug("");
  debug("...LANDED!!!!");
  debug("...LANDED!!!!");
  debug("...LANDED!!!!");
  debug("");
  debug("");
  debug("");
  debug("");

  drawTftReset();
  drawTftSplash();
}

loopThrottle _throttleAirborneDescent;
void loopStateAIRBORNE_DESCENT(unsigned long timestamp, unsigned long deltaElapsed) {
  int delta = _throttleAirborneDescent.determine(deltaElapsed, (int)SAMPLE_RATE_AIRBORNE_DESCENT);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  long currentTimestamp = millis() - _flightLogger.data.timestampLaunch;

  // Determine different in time between the last step...
  long diffTime = currentTimestamp - _flightLogger.data.timestampPrevious;

  float altitudeDelta = loopStateAIRBORNE(currentTimestamp, diffTime);

  bool altitudeCheck = _flightLogger.data.altitudeCurrent < altitudeOffsetGround;
  bool timeoutRecordingCheck = ((timestamp - _flightLogger.data.timestampLaunch) > timeoutRecording);
#ifdef DEBUG_ALTIMETER
  debug("loopStateAIRBORNE_DESCENT...altitudeOffsetGround", altitudeOffsetGround);
  debug("loopStateAIRBORNE_DESCENT...altitudeCheck", altitudeCheck);
  debug("loopStateAIRBORNE_DESCENT...timeoutRecordingCheck", timeoutRecordingCheck);
#endif

  if (timeoutRecordingCheck) {
    // Something went wrong and the recordingt timeout was hit, so abort!
    loopStateAIRBORNEToAbort("Exceeded recording timeout!", "AIRBORNE_DESCENT aborted, returning to GROUND!!!!");
    return;
  }

  if (altitudeCheck) {
    _flightLogger.data.altitudeTouchdown = _flightLogger.data.altitudeLast;
    _flightLogger.data.timestampTouchdown = _flightLogger.data.timestampPrevious;
    // Passed the descent touchdown altitude check, so the flight log is ended and return to GROUND
    loopStateAIRBORNEToGROUND();
    return;
  }
}

loopThrottle _throttleGround;
void loopStateGROUND(unsigned long timestamp, unsigned long deltaElapsed) {
  // Query the button handler to check for button press activity.
  handleButtonLoop();

  // check wifi...
  _wifi.loop();

  // battery
  // voltage = analogReadMilliVolts(10)/500;
  // memmove(&voltage[0], &voltage[1], (voltage_array_capacity - 1) * sizeof(voltage[0]));
  // voltage[voltage_array_capacity - 1] = analogReadMilliVolts(10)/500;

  // Capture the command buffer.
  if (readSerial(timestamp, deltaElapsed))
    interpretCommandBuffer();  // TODO: It'd be nice to kick this to the other processor...

  // Determine the ground loop time delay based on sampling rate.
  int delta = _throttleGround.determine(deltaElapsed, (int)SAMPLE_RATE_GROUND);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  debug("stateGROUND...timestamp", timestamp);

  // Get the current altitude and determine the delta from initial.
  float altitude = readSensorAltitude();
  float altitudeDelta = altitude - _flightLogger.data.altitudeInitial;
  _flightLogger.data.altitudeCurrent = altitude;

#ifdef DEBUG_ALTIMETER
  // debug("stateGROUND...processing, delta", delta);
  // debug("stateGROUND...processing, deltaElapsed", deltaElapsed);
  debug("stateGROUND...altitude", altitude);
  debug("stateGROUND...altitudeInitial", _flightLogger.data.altitudeInitial);
  debug("stateGROUND...altitudeDelta", altitudeDelta);
  debug("stateGROUND...altitudeCurrent", _flightLogger.data.altitudeCurrent);
#endif

  // Check for whether we've left the ground
  // If the delta altitude is less than the specified liftoff altitude, then its on the ground.
  // Lift altitude is a measurement of the difference between the initial altitude and current altitude.
#ifdef DEBUG_ALTIMETER
  debug("stateGROUND...altitudeOffsetLiftoff", altitudeOffsetLiftoff);
#endif
  if (altitudeDelta > altitudeOffsetLiftoff) {
    // Transition to the AIRBORNE_ASCENT ascent stage.
    loopStateGROUNDToAIRBORNE_ASCENT(timestamp);
    return;
  }
}

void loopStateGROUNDToAIRBORNE_ASCENT(unsigned long timestamp) {
  // Turn off wifi, we don't need it in the air...
  setupNetworkDisable();
  
  debug("");
  debug("");
  debug("");
  debug("");
  debug("...AIRBORNE!!!!");
  debug("...AIRBORNE!!!!");
  debug("...AIRBORNE!!!!");
  debug("");
  debug("");
  debug("");
  debug("");

  // Re-initialize the flight...
  _flightLogger.initFlight(timestamp);

  drawTftReset();
  drawTftFlightAirborneStart();

  _loopState.current = AIRBORNE_ASCENT; // TODO!
}

void loop() {
  // put your main code here, to run repeatedly

  unsigned long current = millis();
  unsigned long delta = current - _timestamp;

  // Simple state machine for the flight...

  // Serial.println(F("state..."));
  // Serial.println(_loopState.current);
  // Ground state
  if (_loopState.current == GROUND) {
    // Serial.println(F("state...GROUND"));
    // Only blink while on the ground!
    _neoPixelBlinker.blink(current, 500);
    // Run the ground state algorithms
    loopStateGROUND(current, delta);
  }
  if (_loopState.current == AIRBORNE_ASCENT) {
    // Serial.println(F("state...AIRBORNE_ASCENT"));
    // Only blink while on the ground!
    _neoPixelBlinker.off();
    // Run the airborne ascent state algorithms
    loopStateAIRBORNE_ASCENT(current, delta);
  }
  if (_loopState.current == AIRBORNE_DESCENT) {
    // Serial.println(F("state...AIRBORNE_DESCENT"));
    // Only blink while on the ground!
    _neoPixelBlinker.off();
    // Run the airborne descent state algorithms
    loopStateAIRBORNE_DESCENT(current, delta);
  }

  _timestamp = current;
}

void setupFlightLogger() {
  Serial.println("Setup flight logger...");

  Serial.println("Setup file system...");

  if (!_flightLogger.instance.initFileSystem()) {
    Serial.println("Failed to initialize file system");
    return;
  }

  Serial.println("...file system successful.");

  Serial.println("Initialize flight logger...");

  _flightLogger.instance.initFlight();

  Serial.println("...flight logger successful.");
}

void setupFlightLoggerInitialAtmosphere() {
  Serial.println("Setup initial atmosphere...");
  
  atmosphereValues values = initializeSensorsAtmosphere();
  _flightLogger.data.altitudeInitial = values.altitude;
  Serial.print("initial altitude: ");
  Serial.println(_flightLogger.data.altitudeInitial);

  _flightLogger.data.pressureInitial = values.pressure;
  Serial.print("initial pressure: ");
  Serial.println(_flightLogger.data.pressureInitial);

  _flightLogger.data.temperatureInitial = values.temperature;
  Serial.print("initial temperature: ");
  Serial.println(_flightLogger.data.temperatureInitial);

  Serial.println("...initial atmosphere initialized.");
}

// void setupNetwork() {
//   _ble.start();
//   _wifi.start();
// }

void setupNetworkDisable() {
  // _ble.disable();
  _wifi.disable();
}

void setup() {
  // put your setup code here, to run once:
  Serial.println(F("Setup..."));
  Serial.println("");

  setupLedBuiltin();

  setupNeoPixelBlinker();

  turnOnLedBuiltin();

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

  turnOffLedBuiltin();

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
  Serial.println("");

  Serial.println(F("Memory Available"));
  Serial.print(F("\tFree Heap"));
  Serial.print(F("\tFree Internal Heap"));
  Serial.println(F("\tFree Minimum Heap"));
  Serial.print(F("\t"));
  Serial.print(esp_get_free_heap_size() / 1000);
  Serial.print(F("kb\t\t"));
  Serial.print(esp_get_free_internal_heap_size() / 1000);
  Serial.print(F("kb\t\t\t"));
  Serial.print(esp_get_minimum_free_heap_size() / 1000);
  Serial.println(F("kb"));

  Serial.println("");
  Serial.println(F("...finished."));
  Serial.println(F(""));
}