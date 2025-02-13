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
#include "leds.h"
#include "loopThrottle.h"
#include "memory.h"
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
  ABORTED,
  AIRBORNE_ASCENT,
  AIRBORNE_DESCENT,
  GROUND,
  LANDED
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

int _countdownAborted;
loopThrottle _throttleAborted;
void loopStateABORTED(unsigned long timestamp, unsigned long deltaElapsed) {
  // Determine the aborted loop time delay based on sampling rate.
  int delta = _throttleAborted.determine(deltaElapsed, (int)SAMPLE_RATE_ABORTED);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  _neoPixelBlinker.blink(0xFF0000);

  // Transition to the GROUND stage.
  if (_countdownAborted > SAMPLE_MEASURES_ABORTED) {
    loopStateABORTEDToGROUND(timestamp);
    return;
  }

  _countdownAborted++;
}

void loopStateABORTEDToGROUND(unsigned long timestamp) {
  _loopState.current = GROUND;

  loopStateToGROUND();
}

void loopStateAIRBORNEToAbort(char message1[], char message2[]) {
  // Something went wrong and aborting...

  _flightLogger.data.airborne = false;
  _flightLogger.aborted = true;
  _flightLogger.recording = false;
  _flightLogger.data.touchdown = true;

  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F("...ABORTED!!!!"));
  debug(F("...ABORTED!!!!"));
  Serial.println(message1);
  Serial.println(message2);
  debug(F("...ABORTED!!!!"));
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F(""));

  _countdownAborted = 0;
  
  _loopState.current = ABORTED;
}

float loopStateAIRBORNE(unsigned long currentTimestamp, long diffTime) {
  atmosphereValues atmosphere = readSensorAtmosphere();
  float altitude = readSensorAltitude(atmosphere);
  float altitudeDelta = altitude - _flightLogger.data.altitudeLast;
  _flightLogger.data.altitudeCurrent = altitude;
#ifdef DEBUG_ALTIMETER
  debug(F("loopStateAIRBORNE...altitude"), altitude);
  debug(F("loopStateAIRBORNE...altitudeDelta"), altitudeDelta);
  debug(F("loopStateAIRBORNE...altitudeCurrent"), _flightLogger.data.altitudeCurrent);
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

  // Functionality that happen on the tick goes here:

  _neoPixelBlinker.blink(0x00FF00);

  long currentTimestamp = timestamp - _flightLogger.data.timestampLaunch;
#ifdef DEBUG_ALTIMETER
  debug(F("loopStateAIRBORNE_ASCENT...timestamp"), timestamp);
  debug(F("loopStateAIRBORNE_ASCENT...currentTimestamp"), currentTimestamp);
#endif

  // Determine different in time between the last step...
  long diffTime = currentTimestamp - _flightLogger.data.timestampPrevious;

  float altitudeDelta = loopStateAIRBORNE(currentTimestamp, diffTime);

  // Detect apogee by taking X number of measures as long as the current is less
  // than the last recorded altitude.
#ifdef DEBUG_ALTIMETER
  debug(F("loopStateAIRBORNE_ASCENT...altitudeLast"), _flightLogger.data.altitudeLast);
  debug(F("loopStateAIRBORNE_ASCENT...altitudeDelta"), altitudeDelta);
#endif
  if (altitudeDelta < 0) {
    if (_flightLogger.data.measures == SAMPLE_MEASURES_APOGEE) {
      // Detected apogee.
      _flightLogger.data.altitudeApogeeFirstMeasure = _flightLogger.data.altitudeLast;
      _flightLogger.data.timestampApogeeFirstMeasure = currentTimestamp;
    }
    _flightLogger.data.measures = _flightLogger.data.measures - 1;
#ifdef DEBUG_ALTIMETER
    debug(F("loopStateAIRBORNE_ASCENT...measures"), _flightLogger.data.measures);
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
      debug(F("loopStateAIRBORNE_ASCENT...measures"), F("reset"));
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
  debug(F("loopStateAIRBORNE_ASCENT...timeOutTimeToApogee"), timeOutTimeToApogee);
  debug(F("loopStateAIRBORNE_ASCENT...currentTimestamp"), currentTimestamp);
  debug(F("loopStateAIRBORNE_ASCENT...timestampApogeeCheck"), timestampApogeeCheck);
#endif
  if (timestampApogeeCheck) {
    // Something went wrong and apogee was never found, so abort!
    loopStateAIRBORNEToAbort("Time to apogee threshold exceeded!", "AIRBORNE_ASCENT aborted, returning to GROUND!!!!");
    return;
  }

  bool timeoutRecordingCheck = currentTimestamp > timeoutRecording;
#ifdef DEBUG_ALTIMETER
  debug(F("loopStateAIRBORNE_ASCENT...timestampLaunch"), _flightLogger.data.timestampLaunch);
  debug(F("loopStateAIRBORNE_ASCENT...timeoutRecording"), timeoutRecording);
  debug(F("loopStateAIRBORNE_ASCENT...currentTimestamp"), currentTimestamp);
  debug(F("loopStateAIRBORNE_ASCENT...timeoutRecordingCheck"), timeoutRecordingCheck);
#endif
  if (timeoutRecordingCheck) {
    // Something went wrong., so abort!
    loopStateAIRBORNEToAbort("Time to apogee threshold exceeded!", "AIRBORNE_ASCENT aborted, returning to GROUND!!!!");
    return;
  }
}

void loopStateAIRBORNE_ASCENTToAIRBORNE_DESCENT() {
  _flightLogger.data.altitudeApogee = _flightLogger.data.altitudeApogeeFirstMeasure;
  _flightLogger.data.timestampApogee = _flightLogger.data.timestampApogeeFirstMeasure;

  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F("...DESCENT!!!!"));
  debug(F("...DESCENT!!!!"));
  debug(F("...DESCENT!!!!"));
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F(""));

  _loopState.current = AIRBORNE_DESCENT;
}

loopThrottle _throttleAirborneDescent;
void loopStateAIRBORNE_DESCENT(unsigned long timestamp, unsigned long deltaElapsed) {
  int delta = _throttleAirborneDescent.determine(deltaElapsed, (int)SAMPLE_RATE_AIRBORNE_DESCENT);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  _neoPixelBlinker.blink(0x0000FF);

  long currentTimestamp = millis() - _flightLogger.data.timestampLaunch;

  // Determine different in time between the last step...
  long diffTime = currentTimestamp - _flightLogger.data.timestampPrevious;

  float altitudeDelta = loopStateAIRBORNE(currentTimestamp, diffTime);

  bool altitudeCheck = _flightLogger.data.altitudeCurrent < altitudeOffsetGround;
  bool timeoutRecordingCheck = ((timestamp - _flightLogger.data.timestampLaunch) > timeoutRecording);
#ifdef DEBUG_ALTIMETER
  debug(F("loopStateAIRBORNE_DESCENT...altitudeOffsetGround"), altitudeOffsetGround);
  debug(F("loopStateAIRBORNE_DESCENT...altitudeCheck"), altitudeCheck);
  debug(F("loopStateAIRBORNE_DESCENT...timeoutRecordingCheck"), timeoutRecordingCheck);
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
    loopStateAIRBORNE_DESCENTToLANDED();
    return;
  }
}

void loopStateAIRBORNE_DESCENTToLANDED() {
  // Complete the flight
  Serial.println(F("Flight has ended!!!"));

  _flightLogger.data.airborne = false;
  _flightLogger.recording = false;

  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F("...LANDED!!!!"));
  debug(F("...LANDED!!!!"));
  debug(F("...LANDED!!!!"));
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F(""));

  _loopState.current = LANDED;
}

int _countdownLanded = 0;
loopThrottle _throttleLanded;
void loopStateLANDED(unsigned long timestamp, unsigned long deltaElapsed) {
  // Determine the ground loop time delay based on sampling rate.
  int delta = _throttleLanded.determine(deltaElapsed, (int)SAMPLE_RATE_LANDED);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  _neoPixelBlinker.blink(0xFF00FF);

  // debug(F("loopStateLANDED...timestamp"), timestamp);

  // Transition to the AIRBORNE_ASCENT ascent stage.
  if (_countdownLanded > SAMPLE_MEASURES_LANDED) {
    loopStateLANDEDToGROUND(timestamp);
    return;
  }

  _countdownLanded++;
}

void loopStateLANDEDToGROUND(unsigned long timestamp) {
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F("...GROUNDED!!!!"));
  debug(F("...GROUNDED!!!!"));
  debug(F("...GROUNDED!!!!"));
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F(""));

  _loopState.current = GROUND;
  _countdownAborted = 0;
  _countdownLanded = 0;

  loopStateToGROUND();
}

void loopStateToGROUND() {
  // To avoid battery drain, etc. user should choose to turn on networking...
  // setupNetwork();

  drawTftReset();
  drawTftSplash();
}

loopThrottle _throttleGround;
void loopStateGROUND(unsigned long timestamp, unsigned long deltaElapsed) {
  
  // Only blink while on the ground!
  _neoPixelBlinker.blink(timestamp, 500);

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

  // debug(F("stateGROUND...timestamp"), timestamp);

  // Get the current altitude and determine the delta from initial.
  float altitude = readSensorAltitude();
  float altitudeDelta = altitude - _flightLogger.data.altitudeInitial;
  _flightLogger.data.altitudeCurrent = altitude;

#ifdef DEBUG_ALTIMETER
  // debug(F("stateGROUND...processing, delta"), delta);
  // debug(F("stateGROUND...processing, deltaElapsed"), deltaElapsed);
  debug(F("stateGROUND...altitude"), altitude);
  debug(F("stateGROUND...altitudeInitial"), _flightLogger.data.altitudeInitial);
  debug(F("stateGROUND...altitudeDelta"), altitudeDelta);
  debug(F("stateGROUND...altitudeCurrent"), _flightLogger.data.altitudeCurrent);
#endif

  // Check for whether we've left the ground
  // If the delta altitude is less than the specified liftoff altitude, then its on the ground.
  // Lift altitude is a measurement of the difference between the initial altitude and current altitude.
#ifdef DEBUG_ALTIMETER
  debug(F("stateGROUND...altitudeOffsetLiftoff"), altitudeOffsetLiftoff);
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

  _neoPixelBlinker.off();
  
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F("...AIRBORNE!!!!"));
  debug(F("...AIRBORNE!!!!"));
  debug(F("...AIRBORNE!!!!"));
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F(""));

  // Re-initialize the flight...
  _flightLogger.initFlight(timestamp);

  drawTftReset();
  drawTftFlightAirborneStart();

  _loopState.current = AIRBORNE_ASCENT;
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

  // Simple state machine for the flight...

  // Serial.println(F("state..."));
  // Serial.println(_loopState.current);
  // Ground state
  if (_loopState.current == ABORTED) {
    // Serial.println(F("state...ABORTED"));
    // Run the aborted state algorithms
    loopStateABORTED(current, delta);
  }
  else if (_loopState.current == AIRBORNE_ASCENT) {
    // Serial.println(F("state...AIRBORNE_ASCENT"));
    // Run the airborne ascent state algorithms
    loopStateAIRBORNE_ASCENT(current, delta);
  }
  else if (_loopState.current == AIRBORNE_DESCENT) {
    // Serial.println(F("state...AIRBORNE_DESCENT"));
    // Run the airborne descent state algorithms
    loopStateAIRBORNE_DESCENT(current, delta);
  }
  else if (_loopState.current == GROUND) {
    // Serial.println(F("state...GROUND"));
    // Run the ground state algorithms
    loopStateGROUND(current, delta);
  }
  else if (_loopState.current == LANDED) {
    // Serial.println(F("state...LANDED"));
    // Run the landed state algorithms
    loopStateLANDED(current, delta);
  }

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

void setupNetworkDisable() {
  // _ble.disable();
  _wifi.disable();
}

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