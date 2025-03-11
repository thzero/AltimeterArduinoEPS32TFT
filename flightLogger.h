#ifndef _FLIGHT_LOGGER_H
#define _FLIGHT_LOGGER_H

#include "flightLoggerBase.h"
#include "flightLoggerData.h"
#include "flightLoggerLFS.h"

class flightLogger {
  public:
    flightLogger();
    void init(unsigned long timestamp);
    void reindexFlights();
    void reset();
    
    bool airborne = false;
    bool aborted;
    float altitudeInitial;
    flightLoggerLFS instance;
    // Number of measures to do so that we are sure that apogee has been reached
    unsigned long measures = 5;
    float pressureInitial;
    bool recording;
    float temperatureInitial;
    bool touchdown = false;

  private:
    TaskHandle_t reindexFlightsTaskHandle;

    static void reindexFlightsTaskW(void * parameter);
};

extern flightLogger _flightLogger;

extern void setupFlightLogger();

#endif