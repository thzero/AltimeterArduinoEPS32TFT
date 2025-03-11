#ifndef _FLIGHT_LOGGER_LFS_H
#define _FLIGHT_LOGGER_LFS_H

#include <ArduinoJson.h>

#include "flightLoggerBase.h"
#include "flightLoggerData.h"

#define FORMAT_LITTLEFS_IF_FAILED true

class flightLoggerLFS : public flightLoggerBase {
  public:
    flightLoggerLFS();
    bool eraseFlights() override;
    bool eraseLast();
    bool exists(int flightNbr);
    // flightDataNumberStruct geFlightNbrs();
    // long geFlightNbrsLast();
    bool initFileSystem();
    bool listAsJson(JsonArray flightLogs);
    // bool outputSerialList();
    bool readFile(int flightNbr) override;
    flightDataReadResultsStruct readFileAsJson(int flightNbr);
    int reindexFlights();
    bool writeFile(int flightNbr);
    bool writeFlightCurrent();
};

#endif