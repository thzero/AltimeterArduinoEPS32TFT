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

  airborne = true;
  touchdown = false;

  instance.initFlight(timestamp);
}

void flightLogger::reset() {
  // Reset the flight...
  aborted = false;
  recording = false;

  airborne = false;
  touchdown = false;

  instance.resetFlight();
}

flightLogger _flightLogger;