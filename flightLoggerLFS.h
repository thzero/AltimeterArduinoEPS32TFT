#ifndef _FLIGHT_LOGGER_LFS_H
#define _FLIGHT_LOGGER_LFS_H

#include <ArduinoJson.h>

#include "flightLoggerBase.h"
#include "flightLoggerData.h"

#define FORMAT_LITTLEFS_IF_FAILED true

class flightLoggerLFS : public flightLoggerBase {
  public:
    flightLoggerLFS();
    bool clearFlights();
    bool exists(int flightNbr);
    long geFlightNbrLast();
    bool initFileSystem();
    bool readFile(int flightNbr);
    flightDataReadResultsStruct readFileAsJson(int flightNbr);
    void readFlightsAsJson(JsonArray flightLogs);
    bool writeFile(int flightNbr);
};

#endif