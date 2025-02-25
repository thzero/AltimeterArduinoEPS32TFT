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
#include "stateMachine.h"
#include "tft.h"
#include "utilities.h"

stateMachine::stateMachine() {
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
  }
  else if (_loopState.current == AIRBORNE_ASCENT) {
    // Serial.println(F("state...AIRBORNE_ASCENT"));
    // Run the airborne ascent state algorithms
    loopStateAIRBORNE_ASCENT(timestamp, delta);
  }
  else if (_loopState.current == AIRBORNE_DESCENT) {
    // Serial.println(F("state...AIRBORNE_DESCENT"));
    // Run the airborne descent state algorithms
    loopStateAIRBORNE_DESCENT(timestamp, delta);
  }
  else if (_loopState.current == GROUND) {
    // Serial.println(F("state...GROUND"));
    // Run the ground state algorithms
    loopStateGROUND(timestamp, delta);
  }
  else if (_loopState.current == LANDED) {
    // Serial.println(F("state...LANDED"));
    // Run the landed state algorithms
    loopStateLANDED(timestamp, delta);
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
}

void stateMachine::loopStateABORTEDToGROUND(unsigned long timestamp) {
  _loopState.current = GROUND;

  loopStateToGROUND();
}

void stateMachine::loopStateAIRBORNEToAbort(char message1[], char message2[]) {
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

float stateMachine::loopStateAIRBORNE(unsigned long currentTimestamp, long diffTime) {
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
  atmosphereValues atmosphereValuesO = readSensorAtmosphere();
  _flightLogger.instance.setFlightHumidity(atmosphereValuesO.temperature);
  _flightLogger.instance.setFlightTemperature(atmosphereValuesO.temperature);
  _flightLogger.instance.setFlightPressure(atmosphereValuesO.pressure);

  // Log the flight x, y, and z accelerations...
  
  accelerometerValues accelerometerValuesO = readSensorAccelerometer();
  _flightLogger.instance.setFlightAccelX(accelerometerValuesO.x);
  _flightLogger.instance.setFlightAccelY(accelerometerValuesO.y);
  _flightLogger.instance.setFlightAccelZ(accelerometerValuesO.z);
  
  gyroscopeValues gyroscopeValuesO = readSensorGyroscope();
  _flightLogger.instance.setFlightGyroX(gyroscopeValuesO.x);
  _flightLogger.instance.setFlightGyroY(gyroscopeValuesO.y);
  _flightLogger.instance.setFlightGyroZ(gyroscopeValuesO.z);

  // Add to the set data to the flight.
  _flightLogger.instance.setFlightData();

  _flightLogger.data.timestampPrevious = currentTimestamp;
  _flightLogger.data.altitudeLast = altitude;

  // sendTelemetry(timestamp - _flightLogger.data.timestampInitial , atmosphere, accelerometer, gryoscope, altitude, diffTime);

  drawTftFlightAirborne(currentTimestamp, diffTime);

  return altitudeDelta;
}

void stateMachine::loopStateAIRBORNE_ASCENT(unsigned long timestamp, unsigned long deltaElapsed) {
  int delta = _throttleAirborneAscent.determine(deltaElapsed, _sampleRateAirborneAscent);
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

void stateMachine::loopStateAIRBORNE_ASCENTToAIRBORNE_DESCENT() {
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

void stateMachine::loopStateAIRBORNE_DESCENT(unsigned long timestamp, unsigned long deltaElapsed) {
  int delta = _throttleAirborneDescent.determine(deltaElapsed, _sampleRateAirborneDescent);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  _neoPixelBlinker.blink(0x0000FF);

  long currentTimestamp = millis() - _flightLogger.data.timestampLaunch;

  // Determine different in time between the last step...
  long diffTime = currentTimestamp - _flightLogger.data.timestampPrevious;

  float altitudeDelta = loopStateAIRBORNE(currentTimestamp, diffTime);

  bool altitudeCheck = _flightLogger.data.altitudeCurrent < _altitudeGround;
  bool timeoutRecordingCheck = ((timestamp - _flightLogger.data.timestampLaunch) > timeoutRecording);
#ifdef DEBUG_ALTIMETER
  debug(F("loopStateAIRBORNE_DESCENT...altitudeGround"), _altitudeGround);
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

void stateMachine::loopStateAIRBORNE_DESCENTToLANDED() {
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
  _flightLogger.initFlight(timestamp);

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

void stateMachine::save(int launchDetect, int samplesAscent, int samplesDescent, int samplesGround) {
  Serial.println(F("Save state machine..."));

  Preferences preferences;
  preferences.begin(PREFERENCE_KEY, false);
  _altitudeLiftoff = launchDetect;
  preferences.putInt(PREFERENCE_KEY_LAUNCH_DETECT, _altitudeLiftoff);
  _sampleRateAirborneAscent = samplesAscent;
  preferences.putInt(PREFERENCE_KEY_ALTITUDE_AIRBORNE_ASCENT, _sampleRateAirborneAscent);
  _sampleRateAirborneDescent = samplesDescent;
  preferences.putInt(PREFERENCE_KEY_ALTITUDE_AIRBORNE_DESCENT, _sampleRateAirborneDescent);
  _sampleRateGround = samplesGround;
  preferences.putInt(PREFERENCE_KEY_ALTITUDE_GROUND, _sampleRateGround);
  preferences.end();

  _altitudeGround = _altitudeLiftoff / 2;

  #ifdef DEBUG
  Serial.println(F("\t...state machine... state"));
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
  // preferences.putInt("altitudeLiftoff", _altitudeLiftoff);
  _sampleRateAirborneAscent = preferences.getInt(PREFERENCE_KEY_ALTITUDE_AIRBORNE_ASCENT, (int)SAMPLE_RATE_AIRBORNE_ASCENT);
  // preferences.putInt("sampleRateAirborneAscent", _sampleRateAirborneAscent);
  _sampleRateAirborneDescent = preferences.getInt(PREFERENCE_KEY_ALTITUDE_AIRBORNE_DESCENT, (int)SAMPLE_RATE_AIRBORNE_DESCENT);
  // preferences.putInt("sampleRateAirborneDescent", _sampleRateAirborneDescent);
  _sampleRateGround = preferences.getInt(PREFERENCE_KEY_ALTITUDE_GROUND, (int)SAMPLE_RATE_GROUND);
  // preferences.putInt("sampleRateGround", _sampleRateGround);
  preferences.end();

  #ifdef DEBUG
  Serial.println(F("\t...state machine settings..."));
  _displaySettings();
  Serial.println(F(""));
  #endif

  save(_altitudeLiftoff, _sampleRateAirborneAscent, _sampleRateAirborneDescent, _sampleRateGround);

  Serial.println(F("...state machine setup successful."));
}

void stateMachine::_displaySettings() {
  Serial.print(F("\taltitudeLiftoff="));
  Serial.print(_altitudeLiftoff);
  Serial.print(F(", default="));
  Serial.println(ALTITUDE_LIFTOFF);
  Serial.print(F("\taltitudeGround="));
  Serial.print(_altitudeGround);
  Serial.print(F(", default="));
  Serial.println(SAMPLE_RATE_GROUND);
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

int stateMachine::launchDetect() {
  return _altitudeLiftoff;
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

stateMachine _stateMachine;