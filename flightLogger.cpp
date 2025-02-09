#include "constants.h"
#include "flightLoggerBase.h"
#include "flightLogger.h"

flightLogger::flightLogger() {
  reset();
}

void flightLogger::initFlight(unsigned long timestamp) {
  // Initialize the flight...
  aborted = false;
  recording = true;

  data.airborne = true;
  data.altitudeApogee = 0;
  data.altitudeApogeeFirstMeasure = 0;
  data.altitudeCurrent = 0;
  data.altitudeLaunch = 0;
  data.altitudeLast = 0;
  data.altitudeTouchdown = 0;
  data.measures = SAMPLE_MEASURES_APOGEE;
  data.temperatureInitial = 0;
  data.timestampApogee = 0;
  data.timestampApogeeFirstMeasure = 0;
  data.timestampLaunch = timestamp;
  data.timestampPrevious = 0;
  data.timestampTouchdown = 0;
  data.touchdown = false;

  instance.initFlight();
}

void flightLogger::reset() {
  // Reset the flight...
  aborted = false;
  recording = false;

  data.airborne = false;
  data.altitudeApogee = 0;
  data.altitudeCurrent = 0;
  data.altitudeLaunch = 0;
  data.altitudeLast = 0;
  data.altitudeTouchdown = 0;
  data.measures = SAMPLE_MEASURES_APOGEE;
  data.temperatureInitial = 0;
  data.timestampApogee = 0;
  data.timestampLaunch = 0;
  data.timestampPrevious = 0;
  data.timestampTouchdown = 0;
  data.touchdown = false;
}

flightLogger _flightLogger;