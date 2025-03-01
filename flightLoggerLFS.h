#ifndef _FLIGHT_LOGGER_LFS_H
#define _FLIGHT_LOGGER_LFS_H

#include "flightLoggerBase.h"
#include "flightLoggerData.h"

#define FORMAT_LITTLEFS_IF_FAILED true

class flightLoggerLFS : public flightLoggerBase {
  public:
    flightLoggerLFS();
    bool clearFlightList();
    long geFlightNbrLast();
    bool initFileSystem();
    bool readFlight(int flightNbr);
    bool writeFlight(int flightNbr);
};

#endif