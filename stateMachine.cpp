#include <Arduino.h>
#include <Preferences.h>

#include "button.h"
#include "commands.h"
#include "constants.h"
#include "debug.h"
#include "flightLogger.h"
#include "leds.h"
#include "neoPixel.h"
#include "network.h"
#include "sensor.h"
#include "simulation.h"
#include "stateMachine.h"
#include "time.h"
#include "tft.h"
#include "utilities.h"

stateMachine::stateMachine() {
}

int stateMachine::launchDetect() {
  return _altitudeLiftoff;
}

void stateMachine::loop(unsigned long timestamp, unsigned long delta) {
  // Simple state machine for the flight...

  // Serial.println(F("state..."));
  // Serial.println(_loopState.current);
  // Ground state
  if (_loopState.current == ABORTED) {
    // Serial.println(F("state...ABORTED"));
    // Run the aborted state algorithms
    loopStateABORTED(timestamp, delta);
    return;
  }
  if (_loopState.current == AIRBORNE_ASCENT) {
    // Serial.println(F("state...AIRBORNE_ASCENT"));
    // Run the airborne ascent state algorithms
    loopStateAIRBORNE_ASCENT(timestamp, delta);
    return;
  }
  if (_loopState.current == AIRBORNE_DESCENT) {
    // Serial.println(F("state...AIRBORNE_DESCENT"));
    // Run the airborne descent state algorithms
    loopStateAIRBORNE_DESCENT(timestamp, delta);
    return;
  }
  if (_loopState.current == GROUND) {
    // Serial.println(F("state...GROUND"));
    // Run the ground state algorithms
    loopStateGROUND(timestamp, delta);
    return;
  }
  if (_loopState.current == LANDED) {
    // Serial.println(F("state...LANDED"));
    // Run the landed state algorithms
    loopStateLANDED(timestamp, delta);
    return;
  }
}

void stateMachine::loopStateABORTED(unsigned long timestamp, unsigned long deltaElapsed) {
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
  debug(F("countdownAborted"), _countdownAborted); // TODO
}

void stateMachine::loopStateABORTEDToGROUND(unsigned long timestamp) {
  _loopState.current = GROUND;

  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F("...ABORTED to GROUND!!!!"));
  debug(F("...ABORTED to GROUND!!!!"));
  debug(F("...ABORTED to GROUND!!!!"));
  debug(F(""));
  debug(F(""));
  debug(F(""));
  debug(F(""));

  loopStateToGROUND();
}

void stateMachine::loopStateAIRBORNEToABORTED(char message1[], char message2[]) {
  // Something went wrong and aborting...

  _flightLogger.aborted = true;
  _flightLogger.airborne = false;
  _flightLogger.recording = false;
  _flightLogger.touchdown = false;

#ifdef DEV
  if (_simulation.isRunning()) 
    _simulation.stop();
#endif

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

float stateMachine::loopStateAIRBORNE(unsigned long currentTimestamp, long diffTime) {
  atmosphereValues atmosphere = readSensorAtmosphere();
  float altitude = readSensorAltitude(atmosphere);
  float altitudeDelta = altitude - _flightLogger.instance.getData().altitudeLast;
  _flightLogger.instance.setAltitudeCurrent(altitude);
#ifdef DEBUG_ALTIMETER
  debug(F("loopStateAIRBORNE...altitude"), altitude);
  debug(F("loopStateAIRBORNE...altitudeDelta"), altitudeDelta);
  debug(F("loopStateAIRBORNE...altitudeCurrent"), _flightLogger.instance.getData().altitudeCurrent);
#endif
  _flightLogger.instance.setTimestampCurrent(currentTimestamp);

  // Log the flight altitude...
  _flightLogger.instance.setTraceCurrentTime(diffTime);
  _flightLogger.instance.setTraceCurrentAltitude(_flightLogger.instance.getData().altitudeCurrent);

  // Log the flight temperature and pressure...
  atmosphereValues atmosphereValuesO = readSensorAtmosphere();
  _flightLogger.instance.setTraceCurrentHumidity(atmosphereValuesO.temperature);
  _flightLogger.instance.setTraceCurrentTemperature(atmosphereValuesO.temperature);
  _flightLogger.instance.setTraceCurrentPressure(atmosphereValuesO.pressure);

  // Log the flight x, y, and z accelerations...
  
  accelerometerValues accelerometerValuesO = readSensorAccelerometer();
  _flightLogger.instance.setTraceCurrentAccelX(accelerometerValuesO.x);
  _flightLogger.instance.setTraceCurrentAccelY(accelerometerValuesO.y);
  _flightLogger.instance.setTraceCurrentAccelZ(accelerometerValuesO.z);
  
  gyroscopeValues gyroscopeValuesO = readSensorGyroscope();
  _flightLogger.instance.setTraceCurrentGyroX(gyroscopeValuesO.x);
  _flightLogger.instance.setTraceCurrentGyroY(gyroscopeValuesO.y);
  _flightLogger.instance.setTraceCurrentGyroZ(gyroscopeValuesO.z);

  // Add to the set data to the flight.
  _flightLogger.instance.copyTraceCurrentToArray();

  _flightLogger.instance.setAltitudeLast(altitude);
  _flightLogger.instance.setTimestampPrevious(currentTimestamp);

  // sendTelemetry(timestamp - _flightLogger.instance.getData().timestampInitial , atmosphere, accelerometer, gryoscope, altitude, diffTime);

  drawTftFlightAirborne(currentTimestamp, diffTime);

  return altitudeDelta;
}

void stateMachine::loopStateAIRBORNE_ASCENT(unsigned long timestamp, unsigned long deltaElapsed) {
  int delta = _throttleAirborneAscent.determine(deltaElapsed, _sampleRateAirborneAscent);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  _neoPixelBlinker.blink(0x00FF00);

  long currentTimestamp = timestamp - _flightLogger.instance.getData().timestampLaunch;
#ifdef DEBUG_ALTIMETER
  debug(F("loopStateAIRBORNE_ASCENT...timestamp"), timestamp);
  debug(F("loopStateAIRBORNE_ASCENT...currentTimestamp"), currentTimestamp);
#endif

  // Determine different in time between the last step...
  long diffTime = currentTimestamp - _flightLogger.instance.getData().timestampPrevious;

  float altitudeDelta = loopStateAIRBORNE(currentTimestamp, diffTime);

  // Detect apogee by taking X number of measures as long as the current is less
  // than the last recorded altitude.
#ifdef DEBUG_ALTIMETER
  debug(F("loopStateAIRBORNE_ASCENT...altitudeLast"), _flightLogger.instance.getData().altitudeLast);
  debug(F("loopStateAIRBORNE_ASCENT...altitudeDelta"), altitudeDelta);
#endif
  if (altitudeDelta < 0) {
    if (_flightLogger.measures == SAMPLE_MEASURES_APOGEE) {
      // Detected apogee.
      _flightLogger.instance.setAltitudeApogeeFirstMeasure(_flightLogger.instance.getData().altitudeLast);
      _flightLogger.instance.setTimestampApogeeFirstMeasure(currentTimestamp);
    }
    _flightLogger.measures = _flightLogger.measures - 1;
#ifdef DEBUG_ALTIMETER
    debug(F("loopStateAIRBORNE_ASCENT...measures"), _flightLogger.measures);
#endif
    if (_flightLogger.measures == 0) {
      // Detected apogee.
      loopStateAIRBORNE_ASCENTToAIRBORNE_DESCENT();
      return;
    }
  } 
  else {
    if (_flightLogger.measures < SAMPLE_MEASURES_APOGEE) {
#ifdef DEBUG_ALTIMETER
      debug(F("loopStateAIRBORNE_ASCENT...measures"), F("reset"));
#endif
      // If the curent is greater than the last, then reset as it was potentially
      // a false positive.
      _flightLogger.instance.setAltitudeApogeeFirstMeasure(0);
      _flightLogger.instance.setTimestampApogeeFirstMeasure(0);
      _flightLogger.instance.setAltitudeLast(_flightLogger.instance.getData().altitudeCurrent);
      _flightLogger.measures = SAMPLE_MEASURES_APOGEE;
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
    loopStateAIRBORNEToABORTED("Time to apogee threshold exceeded!", "AIRBORNE_ASCENT aborted, returning to GROUND!!!!");
    return;
  }

  bool timeoutRecordingCheck = currentTimestamp > timeoutRecording;
#ifdef DEBUG_ALTIMETER
  debug(F("loopStateAIRBORNE_ASCENT...timestampLaunch"), _flightLogger.instance.getData().timestampLaunch);
  debug(F("loopStateAIRBORNE_ASCENT...timeoutRecording"), timeoutRecording);
  debug(F("loopStateAIRBORNE_ASCENT...currentTimestamp"), currentTimestamp);
  debug(F("loopStateAIRBORNE_ASCENT...timeoutRecordingCheck"), timeoutRecordingCheck);
#endif
  if (timeoutRecordingCheck) {
    // Something went wrong., so abort!
    loopStateAIRBORNEToABORTED("Time to apogee threshold exceeded!", "AIRBORNE_ASCENT aborted, returning to GROUND!!!!");
    return;
  }
}

void stateMachine::loopStateAIRBORNE_ASCENTToAIRBORNE_DESCENT() {
  _flightLogger.instance.setAltitudeApogee(_flightLogger.instance.getData().altitudeApogeeFirstMeasure);
  _flightLogger.instance.setTimestampApogee(_flightLogger.instance.getData().timestampApogeeFirstMeasure);

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

void stateMachine::loopStateAIRBORNE_DESCENT(unsigned long timestamp, unsigned long deltaElapsed) {
  int delta = _throttleAirborneDescent.determine(deltaElapsed, _sampleRateAirborneDescent);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  _neoPixelBlinker.blink(0x0000FF);

  long currentTimestamp = millis() - _flightLogger.instance.getData().timestampLaunch;

  // Determine different in time between the last step...
  long diffTime = currentTimestamp - _flightLogger.instance.getData().timestampPrevious;

  float altitudeDelta = loopStateAIRBORNE(currentTimestamp, diffTime);

  bool altitudeCheck = _flightLogger.instance.getData().altitudeCurrent < _altitudeGround;
  bool timeoutRecordingCheck = ((timestamp - _flightLogger.instance.getData().timestampLaunch) > timeoutRecording);
#ifdef DEBUG_ALTIMETER
  debug(F("loopStateAIRBORNE_DESCENT...altitudeGround"), _altitudeGround);
  debug(F("loopStateAIRBORNE_DESCENT...altitudeCheck"), altitudeCheck);
  debug(F("loopStateAIRBORNE_DESCENT...timeoutRecordingCheck"), timeoutRecordingCheck);
#endif

  if (timeoutRecordingCheck) {
    // Something went wrong and the recordingt timeout was hit, so abort!
    loopStateAIRBORNEToABORTED("Exceeded recording timeout!", "AIRBORNE_DESCENT aborted, returning to GROUND!!!!");
    return;
  }

  if (altitudeCheck) {
  _flightLogger.instance.setAltitudeTouchdown(_flightLogger.instance.getData().altitudeLast);
  _flightLogger.instance.setTimestampTouchdown(_flightLogger.instance.getData().timestampPrevious);
    // Passed the descent touchdown altitude check, so the flight log is ended and return to GROUND
    loopStateAIRBORNE_DESCENTToLANDED();
    return;
  }
}

void stateMachine::loopStateAIRBORNE_DESCENTToLANDED() {
  // Complete the flight
  Serial.println(F("Flight has ended!!!"));

  _flightLogger.airborne = false;
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

void stateMachine::loopStateLANDED(unsigned long timestamp, unsigned long deltaElapsed) {
  // Determine the ground loop time delay based on sampling rate.
  int delta = _throttleLanded.determine(deltaElapsed, (int)SAMPLE_RATE_LANDED);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  _neoPixelBlinker.blink(0xFF00FF);

  // debug(F("loopStateLANDED...timestamp"), timestamp);

  // Transition to the AIRBORNE_ASCENT ascent stage.
  if (_countdownLanded > SAMPLE_MEASURES_LANDED) {
    loopStateLANDEDToGROUND();
    return;
  }

  _countdownLanded++;
}

void stateMachine::loopStateLANDEDToGROUND() {
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

#ifdef DEBUG
  Serial.println(F("\nWriting output..."));
#endif
  _flightLogger.instance.writeFlightCurrent();
#ifdef DEBUG
  Serial.println(F("...writing output.\n"));
#endif

  Serial.println(F("Serial Output"));
  _flightLogger.instance.outputSerial();
  Serial.print(F("\nExpanded Serial Output\n\n"));
  _flightLogger.instance.outputSerialExpanded();
  Serial.print(F("\n"));

  loopStateToGROUND();
}

void stateMachine::loopStateToGROUND() {
  // To avoid battery drain, etc. user should choose to turn on networking...
  // networkStart();

  drawTftReset();
  drawTftSplash();
}

void stateMachine::loopStateGROUND(unsigned long timestamp, unsigned long deltaElapsed) {
  // Only blink while on the ground!
  _neoPixelBlinker.blink(timestamp, 500);
  
  // Query the button handler to check for button press activity.
  handleButtonLoop();

  networkLoop();

  // battery
  // voltage = analogReadMilliVolts(10)/500;
  // memmove(&voltage[0], &voltage[1], (voltage_array_capacity - 1) * sizeof(voltage[0]));
  // voltage[voltage_array_capacity - 1] = analogReadMilliVolts(10)/500;

  // Capture the command buffer.
  if (readSerial(timestamp, deltaElapsed))
    interpretCommandBuffer();  // TODO: It'd be nice to kick this to the other processor...

  // Determine the ground loop time delay based on sampling rate.
  int delta = _throttleGround.determine(deltaElapsed, _sampleRateGround);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  // debug(F("stateGROUND...timestamp"), timestamp);

  // Get the current altitude and determine the delta from initial.
  float altitude = readSensorAltitude();
  float altitudeDelta = altitude - _flightLogger.altitudeInitial;
  _flightLogger.instance.setTimestampTouchdown(_flightLogger.instance.getData().altitudeCurrent);

#ifdef DEBUG_ALTIMETER
  debug(F("stateGROUND...processing, delta"), delta);
  debug(F("stateGROUND...processing, deltaElapsed"), deltaElapsed);
  debug(F("stateGROUND...altitude"), altitude);
  debug(F("stateGROUND...altitudeInitial"), _flightLogger.altitudeInitial);
  debug(F("stateGROUND...altitudeDelta"), altitudeDelta);
  debug(F("stateGROUND...altitudeCurrent"), _flightLogger.instance.getData().altitudeCurrent);
#endif

  // Check for whether we've left the ground
  // If the delta altitude is less than the specified liftoff altitude, then its on the ground.
  // Lift altitude is a measurement of the difference between the initial altitude and current altitude.
#ifdef DEBUG_ALTIMETER
  debug(F("stateGROUND...altitudeLiftoff"), _altitudeLiftoff);
#endif
  if (altitudeDelta > _altitudeLiftoff) {
    // Transition to the AIRBORNE_ASCENT ascent stage.
    loopStateGROUNDToAIRBORNE_ASCENT(timestamp);
    return;
  }
}

void stateMachine::loopStateGROUNDToAIRBORNE_ASCENT(unsigned long timestamp) {
  // Turn off networks, we don't need it in the air...
  networkDisable();

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
  _flightLogger.init(timestamp);

  drawTftReset();
  drawTftFlightAirborneStart();

  _loopState.current = AIRBORNE_ASCENT;
}

void stateMachine::reset() {
  Serial.println(F("Reset state machine..."));

  _altitudeLiftoff = (int)ALTITUDE_LIFTOFF;
  _sampleRateAirborneAscent = (int)SAMPLE_RATE_AIRBORNE_ASCENT;
  _sampleRateAirborneDescent = (int)SAMPLE_RATE_AIRBORNE_DESCENT;
  _sampleRateGround = (int)SAMPLE_RATE_GROUND;
  
  save(_altitudeLiftoff, _sampleRateAirborneAscent, _sampleRateAirborneDescent, _sampleRateGround);

  Serial.println(F("...state machine reset successful."));
}

int stateMachine::sampleRateAirborneAscent() {
  return _sampleRateAirborneAscent;
}
int stateMachine::sampleRateAirborneDescent() {
  return _sampleRateAirborneDescent;
}
int stateMachine::sampleRateGround() {
  return _sampleRateGround;
}

void stateMachine::save(int launchDetect, int sampleRateAirborneAscent, int sampleRateAirborneDecent, int sampleRateGround) {
  Serial.println(F("Save state machine..."));
  
#ifdef DEBUG
  Serial.println(F("\t...state machine... save requests"));
  Serial.print(F("\tlaunchDetect="));
  Serial.println(launchDetect);
  Serial.print(F("\tsampleRateAirborneAscent="));
  Serial.println(sampleRateAirborneAscent);
  Serial.print(F("\tsampleRateAirborneDecent="));
  Serial.println(sampleRateAirborneDecent);
  Serial.print(F("\tsampleRateGround="));
  Serial.println(sampleRateGround);

  Serial.println(F("\t...state machine... save current"));
  _displaySettings();
#endif

  _altitudeLiftoff = _checkValues(launchDetectValues, launchDetect, (int)ALTITUDE_LIFTOFF, sizeof(launchDetectValues) / sizeof(launchDetectValues[0]));
  _sampleRateAirborneAscent = _checkValues(sampleRateAirborneAscentValues, sampleRateAirborneAscent, (int)SAMPLE_RATE_AIRBORNE_ASCENT, sizeof(sampleRateAirborneAscentValues) / sizeof(sampleRateAirborneAscentValues[0]));
  _sampleRateAirborneDescent = _checkValues(sampleRateAirborneDecentValues, sampleRateAirborneDecent, (int)SAMPLE_RATE_AIRBORNE_DESCENT, sizeof(sampleRateAirborneDecentValues) / sizeof(sampleRateAirborneDecentValues[0]));
  _sampleRateGround = _checkValues(sampleRateGroundValues, sampleRateGround, (int)SAMPLE_RATE_GROUND, sizeof(sampleRateGroundValues) / sizeof(sampleRateGroundValues[0]));
  
#ifdef DEBUG
  Serial.println(F("\t...state machine... save checked"));
  Serial.print(F("\t_altitudeLiftoff="));
  Serial.println(_altitudeLiftoff);
  Serial.print(F("\t_sampleRateAirborneAscent="));
  Serial.println(_sampleRateAirborneAscent);
  Serial.print(F("\t_sampleRateAirborneDescent="));
  Serial.println(_sampleRateAirborneDescent);
  Serial.print(F("\t_sampleRateGround="));
  Serial.println(_sampleRateGround);
#endif

  Preferences preferences;
  preferences.begin(PREFERENCE_KEY, false);
  preferences.putInt(PREFERENCE_KEY_LAUNCH_DETECT, _altitudeLiftoff);
  preferences.putInt(PREFERENCE_KEY_ALTITUDE_AIRBORNE_ASCENT, _sampleRateAirborneAscent);
  preferences.putInt(PREFERENCE_KEY_ALTITUDE_AIRBORNE_DESCENT, _sampleRateAirborneDescent);
  preferences.putInt(PREFERENCE_KEY_ALTITUDE_GROUND, _sampleRateGround);
  preferences.end();

  _altitudeGround = _altitudeLiftoff / 2;

#ifdef DEBUG
  Serial.println(F("\t...state machine... saved state"));
  _displaySettings();
  Serial.println(F(""));
#endif

  Serial.println(F("...state machine save successful."));
}

void stateMachine::setup() {
  Serial.println(F("Setup state machine..."));

  Preferences preferences;
  preferences.begin(PREFERENCE_KEY, false);
  _altitudeLiftoff = preferences.getInt(PREFERENCE_KEY_LAUNCH_DETECT, (int)ALTITUDE_LIFTOFF);
  if (_altitudeLiftoff <= 0) {
    _altitudeLiftoff = (int)ALTITUDE_LIFTOFF;
    preferences.putInt(PREFERENCE_KEY_LAUNCH_DETECT, _altitudeLiftoff);
  }
  _sampleRateAirborneAscent = preferences.getInt(PREFERENCE_KEY_ALTITUDE_AIRBORNE_ASCENT, (int)SAMPLE_RATE_AIRBORNE_ASCENT);
  if (_sampleRateAirborneAscent <= 0) {
    _sampleRateAirborneAscent = (int)SAMPLE_RATE_AIRBORNE_ASCENT;
    preferences.putInt(PREFERENCE_KEY_ALTITUDE_AIRBORNE_ASCENT, _sampleRateAirborneAscent);
  }
  _sampleRateAirborneDescent = preferences.getInt(PREFERENCE_KEY_ALTITUDE_AIRBORNE_DESCENT, (int)SAMPLE_RATE_AIRBORNE_DESCENT);
  if (_sampleRateAirborneDescent <= 0) {
    _sampleRateAirborneDescent = (int)SAMPLE_RATE_AIRBORNE_DESCENT;
    preferences.putInt(PREFERENCE_KEY_ALTITUDE_AIRBORNE_DESCENT, _sampleRateAirborneDescent);
  }
  _sampleRateGround = preferences.getInt(PREFERENCE_KEY_ALTITUDE_GROUND, (int)SAMPLE_RATE_GROUND);
  if (_sampleRateGround <= 0) {
    _sampleRateGround = (int)PREFERENCE_KEY_ALTITUDE_GROUND;
    preferences.putInt(PREFERENCE_KEY_ALTITUDE_GROUND, _sampleRateGround);
  }
  preferences.end();

  _altitudeGround = _altitudeLiftoff / 2;

#ifdef DEBUG
  Serial.println(F("\t...state machine settings..."));
  _displaySettings();
  Serial.println(F(""));
#endif

  save(_altitudeLiftoff, _sampleRateAirborneAscent, _sampleRateAirborneDescent, _sampleRateGround);

  Serial.println(F("...state machine setup successful."));
}

int stateMachine::_checkValues(int values[], int value, int defaultValue, int size) {
  if (value < 0)
    return defaultValue;

  for (int i = 0; i < size; i++) {
    if (value == values[i])
      return value;
  }
  
  return defaultValue;
}

void stateMachine::_displaySettings() {
  Serial.print(F("\taltitudeLiftoff="));
  Serial.print(_altitudeLiftoff);
  Serial.print(F(", default="));
  Serial.println(ALTITUDE_LIFTOFF);
  Serial.print(F("\taltitudeGround="));
  Serial.println(_altitudeGround);
  Serial.print(F("\tsampleRateAirborneAscent="));
  Serial.print(_sampleRateAirborneAscent);
  Serial.print(F(", default="));
  Serial.println(SAMPLE_RATE_AIRBORNE_ASCENT);
  Serial.print(F("\tsampleRateAirborneDescent="));
  Serial.print(_sampleRateAirborneDescent);
  Serial.print(F(", default="));
  Serial.println(SAMPLE_RATE_AIRBORNE_DESCENT);
  Serial.print(F("\tsampleRateGround="));
  Serial.print(_sampleRateGround);
  Serial.print(F(", default="));
  Serial.println(SAMPLE_RATE_GROUND);
}

stateMachine _stateMachine;  