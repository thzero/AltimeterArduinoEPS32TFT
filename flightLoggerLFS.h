#ifndef _FLIGHT_LOGGER_LFS_H
#define _FLIGHT_LOGGER_LFS_H

#include <ArduinoJson.h>

#include "flightLoggerBase.h"
#include "flightLoggerData.h"

#define FORMAT_LITTLEFS_IF_FAILED true

class flightLoggerLFS : public flightLoggerBase {
  public:
    flightLoggerLFS();
    bool clearFlightList();
    bool existsFlight(int flightNbr);
    long geFlightNbrLast();
    bool initFileSystem();
    bool readFlight(int flightNbr);
    flightDataReadResultsStruct readFlightAsJson(int flightNbr);
    void readFlightsAsJson(JsonArray flightLogs);
    bool writeFlight(int flightNbr);
};

#endif