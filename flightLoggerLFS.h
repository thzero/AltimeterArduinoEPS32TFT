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
    bool eraseLast() override;
    bool exists(int flightNbr) override;
    bool initFileSystem();
    bool listAsJson(JsonArray flightLogs) override;
    bool readFile(int flightNbr) override;
    bool readFileAsJson(int flightNbr, JsonObject object) override;
    int reindexFlights() override;
    bool writeFile(int flightNbr) override;
    bool writeFlightCurrent() override;
};

#endif