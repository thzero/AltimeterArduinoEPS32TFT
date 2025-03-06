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
    void listAsJson(JsonArray flightLogs);
    void outputSerialList();
    bool readFile(int flightNbr);
    flightDataReadResultsStruct readFileAsJson(int flightNbr);
    bool writeFile(int flightNbr);
};

#endif