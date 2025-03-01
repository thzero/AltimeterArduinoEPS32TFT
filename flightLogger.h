#ifndef _FLIGHT_LOGGER_H
#define _FLIGHT_LOGGER_H

#include "flightLoggerBase.h"
#include "flightLoggerData.h"
#include "flightLoggerLFS.h"

class flightLogger {
  public:
    flightLogger();
    void initFlight(unsigned long timestamp);
    void reset();
    
    bool airborne = false;
    bool aborted;
    float altitudeInitial;
    flightLoggerBase instance = flightLoggerLFS();
    // Number of measures to do so that we are sure that apogee has been reached
    unsigned long measures = 5;
    float pressureInitial;
    bool recording;
    float temperatureInitial;
    bool touchdown = false;

  // private:
};
extern flightLogger _flightLogger;

#endif