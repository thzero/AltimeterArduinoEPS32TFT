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
    bool aborted;
    flightLoggerData data;
    flightLoggerBase instance = flightLoggerLFS();
    bool recording;

  // private:
};
extern flightLogger _flightLogger;

#endif