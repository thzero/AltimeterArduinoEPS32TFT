#ifndef _FLIGHT_LOGGER_LFS_H
#define _FLIGHT_LOGGER_LFS_H

#include "flightLoggerBase.h"
#include "flightLoggerData.h"

#define FORMAT_LITTLEFS_IF_FAILED true

class flightLoggerLFS : public flightLoggerBase {
  public:
    flightLoggerLFS();
    bool addToCurrentFlight();
    bool clearFlightList();
    long getFlightDuration();
    void determineFlightMinAndMax(int flightNbr);
    float getFlightAccelXMax();
    float getFlightAccelXMin();
    float getFlightAccelYMax();
    float getFlightAccelYMin();
    float getFlightAccelZMax();
    float getFlightAccelZMin();
    float getFlightAltitudeMax();
    float getFlightAltitudeMin();
    flightDataStruct* getFlightData();
    float getFlightHumidityMax();
    float getFlightHumidityMin();
    float getFlightPressureMax();
    float getFlightPressureMin();
    float getFlightTemperatureMax();
    float getFlightTemperatureMin();
    float getFlightVelocityMax();
    float getFlightVelocityMin();
    long geFlightNbrLast();
    long getFlightSize();
    bool initFileSystem();
    bool initFlight();
    void printFlightData(int flightNbr);
    bool readFlight(int flightNbr);
    void setFlightAccelX(float accelX);
    void setFlightAccelY(float accelY);
    void setFlightAccelZ(float accelZ);
    void setFlightAltitude(float altitude);
    void setFlightPressure(float pressure);
    void setFlightHumidity(float humidity);
    void setFlightTemperature(float temperature);
    void setFlightVelocity(float velocity);
    void setFlightTime(long diffTime);
    bool writeFlight(int flightNbr);
    bool writeFlightFast();
};

#endif