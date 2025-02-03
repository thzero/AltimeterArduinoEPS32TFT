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
int timeoutRecording = 300 * 1000;
int timeOutTimeToApogee = 2000;

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
*/void sleepDevice() {
  setupButtonDeninit();
  setupNeoPixelBlinkerDeninit();

  drawTftReset();
  drawTftSleep();

  // digitalWrite(4, LOW);
  // delay(2000);

  // neilrbowen...
  // pinMode(TFT_BACKLITE, OUTPUT);
  // pinMode(TFT_I2C_POWER, OUTPUT);
  // pinMode(4, OUTPUT);
  // digitalWrite(TFT_BACKLITE, LOW);
  // digitalWrite(TFT_I2C_POWER, LOW);
  sleepTft();
  delay(100);

  gpio_deep_sleep_hold_en();
  // gpio_hold_en((gpio_num_t)TFT_BACKLITE);
  // gpio_hold_en((gpio_num_t)TFT_I2C_POWER);
  sleepTft2();
  delay(2000);
  // ...neilrbowen

  esp_deep_sleep_start();
}

void loopStateAIRBORNE_ASCENTToAIRBORNE_DESCENT() {
  _loopState.current = AIRBORNE_DESCENT;  // TODO!
}

float loopStateAIRBORNE(unsigned long currentTimestamp, long diffTime) {
  atmosphereValues atmosphere = readSensorAtmosphere();
  float altitude = readSensorAltitude(atmosphere);
  debug("loopStateAIRBORNE...altitude", altitude);
  float altitudeDelta = altitude - _flightLogger.data.altitudeLast;
  debug("loopStateAIRBORNE...altitudeDelta", altitudeDelta);
  _flightLogger.data.altitudeCurrent = altitude;

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

  // Add to the set data to the current flight.
  _flightLogger.instance.addToCurrentFlight();

  _flightLogger.data.timestampPrevious = currentTimestamp;
  _flightLogger.data.altitudeLast = altitude;

  // sendTelemetry(timestamp - _flightLogger.data.timestampInitial , atmosphere, accelerometer, gryoscope, altitude, diffTime);

  drawTftFlightAirborne(currentTimestamp, diffTime);

  return altitude;
}

loopThrottle _throttleAirborneAscent;
void loopStateAIRBORNE_ASCENT(unsigned long timestamp, unsigned long deltaIn) {
  int delta = _throttleAirborneAscent.determine(deltaIn, (int)SAMPLE_RATE_AIRBORNE_ASCENT);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  // atmosphereValues atmosphere = readSensorAtmosphere();
  // float altitude = readSensorAltitude(atmosphere);
  // debug("loopStateAIRBORNE_DESCENT...altitude", altitude);
  // float altitudeDelta = altitude - _flightLogger.data.altitudeLast;
  // debug("loopStateAIRBORNE_DESCENT...altitudeDelta", altitudeDelta);
  // _flightLogger.data.altitudeCurrent = altitude;

  // long currentTimestamp = millis() - _flightLogger.data.timestampLaunch;

  // // Determine different in time between the last step...
  // long diffTime = currentTimestamp - _flightLogger.data.timestampPrevious;

  // // Log the flight altitude...
  // _flightLogger.instance.setFlightTime(diffTime);
  // _flightLogger.instance.setFlightAltitude(_flightLogger.data.altitudeCurrent);

  // // Log the flight temperature and pressure...
  // double temperature, pressure;
  // atmosphereValues val = readSensorAtmosphere();
  // _flightLogger.instance.setFlightHumidity((long)val.temperature);
  // _flightLogger.instance.setFlightTemperature((long)val.temperature);
  // _flightLogger.instance.setFlightPressure((long)val.pressure);

  // // Log the flight x, y, and z accelerations...
  // // if (_qmi.instance.getDataReady()) {
  // //   if (_qmi.instance.getAccelerometer(_qmi.acc.x, _qmi.acc.y, _qmi.acc.z)) {
  // //     _flightLogger.instance.setFlightAccelX(_qmi.acc.x);
  // //     _flightLogger.instance.setFlightAccelY(_qmi.acc.y);
  // //     _flightLogger.instance.setFlightAccelZ(_qmi.acc.z);
  // //   }
  // // }

  // // Add to the set data to the current flight.
  // _flightLogger.instance.addToCurrentFlight();

  // _flightLogger.data.timestampPrevious = currentTimestamp;
  // _flightLogger.data.altitudeLast = altitude;

  // // sendTelemetry(timestamp - _flightLogger.data.timestampInitial , atmosphere, accelerometer, gryoscope, altitude, deltaIn);

  // // Draw the output...
  // drawTftFlightAirborne(timestamp, deltaIn);

  long currentTimestamp = millis() - _flightLogger.data.timestampLaunch;

  // Determine different in time between the last step...
  long diffTime = currentTimestamp - _flightLogger.data.timestampPrevious;

  float altitude = loopStateAIRBORNE(currentTimestamp, diffTime);

  // Detect apogee by taking X number of measures as long as the current is less
  // than the last recorded altitude.
  if (_flightLogger.data.altitudeCurrent < _flightLogger.data.altitudeLast) {
    _flightLogger.data.measures = _flightLogger.data.measures - 1;
    if (_flightLogger.data.measures == 0) {
      // Detected apogee.
      _flightLogger.data.altitudeApogee = _flightLogger.data.altitudeLast;
      _flightLogger.data.timestampApogee = currentTimestamp;
      // _loopState.current = AIRBORNE_DESCENT;
      debug("DESCENT!!!!");
      loopStateAIRBORNE_ASCENTToAIRBORNE_DESCENT();
      return;
    }
  } 
  else {
    // If the curent is greater than the last, then reset as it was potentially
    // a false positive.
    _flightLogger.data.altitudeLast = _flightLogger.data.altitudeCurrent;
    _flightLogger.data.measures = 5;
  }

  // Check for timeouts...
  bool timestampApogeeCheck = (millis() - _flightLogger.data.timestampApogee) > timeOutTimeToApogee;
  debug("loopStateAIRBORNE_ASCENT...timestampApogeeCheck", timestampApogeeCheck);
  bool timeoutRecordingCheck = (millis() - _flightLogger.data.timestampLaunch) > timeoutRecording;
  debug("loopStateAIRBORNE_ASCENT...timeoutRecordingCheck", timeoutRecordingCheck);

  if (timestampApogeeCheck) {
    // Something went wrong and apogee was never found, so abort!
    loopStateAIRBORNEToAbort("Time to apogee threshold exceeded!", "AIRBORNE_ASCENT aborted, returning to GROUND!!!!");
    return;
  }

  if (timeoutRecordingCheck) {
    // Something went wrong., so abort!
    loopStateAIRBORNEToAbort("Time to apogee threshold exceeded!", "AIRBORNE_ASCENT aborted, returning to GROUND!!!!");
    return;
  }
}

void loopStateAIRBORNEToAbort(char message1[], char message2[]) {
  // Something went wrong and aborting...
  Serial.println(message1);
  Serial.println(message2);

  // _loopState.current = GROUND; // TODO
  _flightLogger.data.airborne = false;
  _flightLogger.aborted = true;
  _flightLogger.recording = false;
  _flightLogger.data.touchdown = true;

  // To avoid battery drain, etc. user should choose to turn on networking...
  // setupNetwork();

  drawTftReset();
  drawTftSplash();
}

void loopStateAIRBORNEToGROUND() {
  // Complete the flight
  Serial.println("Flight has ended!!!");

  // _loopState.current = GROUND; // TODO
  _flightLogger.data.airborne = false;
  _flightLogger.recording = false;

  // To avoid battery drain, etc. user should choose to turn on networking...
  // setupNetwork();

  drawTftReset();
  drawTftSplash();
}

loopThrottle _throttleAirborneDescent;
void loopStateAIRBORNE_DESCENT(unsigned long timestamp, unsigned long deltaIn) {
  int delta = _throttleAirborneDescent.determine(deltaIn, (int)SAMPLE_RATE_AIRBORNE_DESCENT);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  // atmosphereValues atmosphere = readSensorAtmosphere();
  // float altitude = readSensorAltitude(atmosphere);
  // debug("loopStateAIRBORNE_DESCENT...altitude", altitude);
  // float altitudeDelta = altitude - _flightLogger.data.altitudeLast;
  // debug("loopStateAIRBORNE_DESCENT...altitudeDelta", altitudeDelta);
  // _flightLogger.data.altitudeCurrent = altitude;

  // // Log the flight x, y, and z accelerations...
  // // if (_qmi.instance.getDataReady()) {
  // //   if (_qmi.instance.getAccelerometer(_qmi.acc.x, _qmi.acc.y, _qmi.acc.z)) {
  // //     _flightLogger.instance.setFlightAccelX(_qmi.acc.x);
  // //     _flightLogger.instance.setFlightAccelY(_qmi.acc.y);
  // //     _flightLogger.instance.setFlightAccelZ(_qmi.acc.z);
  // //   }
  // // }

  // // sendTelemetry(timestamp - _flightLogger.data.timestampInitial , atmosphere, accelerometer, gryoscope, altitude, deltaIn);

  // drawTftFlightAirborne(timestamp, deltaIn);

  long currentTimestamp = millis() - _flightLogger.data.timestampLaunch;

  // Determine different in time between the last step...
  long diffTime = currentTimestamp - _flightLogger.data.timestampPrevious;

  float altitude = loopStateAIRBORNE(currentTimestamp, diffTime);

  debug("loopStateAIRBORNE_DESCENT...altitudeOffsetGround", altitudeOffsetGround);
  bool altitudeCheck = _flightLogger.data.altitudeCurrent < altitudeOffsetGround;
  debug("loopStateAIRBORNE_DESCENT...altitudeCheck", altitudeCheck);
  bool timeoutRecordingCheck = ((timestamp - _flightLogger.data.timestampLaunch) > timeoutRecording);
  debug("loopStateAIRBORNE_DESCENT...timeoutRecordingCheck", timeoutRecordingCheck);

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
void loopStateGROUND(unsigned long timestamp, unsigned long deltaIn) {
  // Query the button handler to check for button press activity.
  handleButtonLoop();

  // check wifi...
  _wifi.loop();

  // Capture the command buffer.
  if (readSerial(timestamp, deltaIn))
    interpretCommandBuffer();  // TODO: It'd be nice to kick this to the other processor...

  // Determine the ground loop time delay based on sampling rate.
  int delta = _throttleGround.determine(deltaIn, (int)SAMPLE_RATE_GROUND);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  debug("stateGROUND...processing, delta", delta);

  // Get the current altitude and determine the delta from initial.
  // float altitude = 0;
  float altitude = readSensorAltitude();
  debug("stateGROUND...altitude", altitude);
  debug("stateGROUND...altitudeInitial", _flightLogger.data.altitudeInitial);
  float altitudeDelta = altitude - _flightLogger.data.altitudeInitial;
  debug("stateGROUND...altitudeDelta", altitudeDelta);
  _flightLogger.data.altitudeCurrent = altitude;
  debug("stateGROUND...altitudeCurrent", _flightLogger.data.altitudeCurrent);

  // // Check for whether we've left the ground
  // // If the delta altitude is less than the specified liftoff altitude, then its on the ground.
  // // Lift altitude is a measurement of the difference between the initial altitude and current altitude.
  // debug("stateGROUND...altitudeOffsetLiftoff", altitudeOffsetLiftoff);
  // if (altitudeDelta > altitudeOffsetLiftoff) {
  //   // left the ground...
  //   debug("AIRBORNE!!!!");
  //   // Transition to the AIRBORNE_ASCENT ascent stage.
  //   loopStateGROUNDToAIRBORNE_ASCENT(timestamp);
  //   return;
  // }
}

void loopStateGROUNDToAIRBORNE_ASCENT(unsigned long timestamp) {
  // _loopState.current = AIRBORNE_ASCENT; // TODO!

  // Turn off wifi, we don't need it in the air...
  setupNetworkDisable();

  // Re-initialize the flight...
  _flightLogger.initFlight(timestamp);
  // _flightLogger.aborted = false;
  // _flightLogger.data.altitudeLast = 0;
  // _flightLogger.data.measures = 0;
  // _flightLogger.instance.initFlight();

  drawTftReset();
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
    // Serial.println("state...GROUND");
    // Only blink while on the ground!
    _neoPixelBlinker.blink(current, 500);
    // Run the ground state algorithms
    loopStateGROUND(current, delta);
  }
  if (_loopState.current == AIRBORNE_ASCENT) {
    // Serial.println("state...AIRBORNE_ASCENT");
    // Only blink while on the ground!
    _neoPixelBlinker.off();
    // Run the airborne ascent state algorithms
    loopStateAIRBORNE_ASCENT(current, delta);
  }
  if (_loopState.current == AIRBORNE_DESCENT) {
    // Serial.println("state...AIRBORNE_DESCENT");
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

// void setupTft() {
//   Serial.println("Setup TFT...");

//   // turn on backlite
//   pinMode(TFT_BACKLITE, OUTPUT);
//   digitalWrite(TFT_BACKLITE, HIGH);

//   // turn on the TFT / I2C power supply
//   pinMode(TFT_I2C_POWER, OUTPUT);
//   digitalWrite(TFT_I2C_POWER, HIGH);
//   delay(10);

//   _tft.init();
//   drawTftReset();
//   _tft.setSwapBytes(true);

//   drawTftSplash();

//   Serial.println("...TFT successful.");
// }

void setup() {
  // put your setup code here, to run once:
  Serial.println(F("Setup..."));

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

  Serial.println(F("...finished."));
  Serial.println(F(""));
}