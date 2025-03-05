#ifndef _FLIGHT_LOGGER_BASE_H
#define _FLIGHT_LOGGER_BASE_H

#include <ArduinoJson.h>

#include "flightLoggerData.h"

class flightLoggerBase {
  public:
    bool clearFlightList();
    bool copyFlightDataTrace();
    void determineFlightMinAndMax(int flightNbr);
    bool existsFlight(int flightNbr);
    float getFlightAccelXMax();
    float getFlightAccelXMin();
    float getFlightAccelYMax();
    float getFlightAccelYMin();
    float getFlightAccelZMax();
    float getFlightAccelZMin();
    float getFlightAltitudeMax();
    float getFlightAltitudeMin();
    flightDataStruct getFlightData();
    flightDataTraceStruct* getFlightDataTrace();
    long getFlightDuration();
    float getFlightHumidityMax();
    float getFlightHumidityMin();
    long geFlightNbrLast();
    float getFlightPressureMax();
    float getFlightPressureMin();
    long getFlightSize();
    float getFlightTemperatureMax();
    float getFlightTemperatureMin();
    float getFlightVelocityMax();
    float getFlightVelocityMin();
    bool initFileSystem();
    void initFlight(unsigned long timestamp);
    void printFlightData(int flightNbr);
    bool readFlight(int flightNbr);
    JsonObject readFlightAsJson(int flightNbr);
    void readFlightsAsJson(JsonArray flightLogs);
    void resetFlight();
    void setFlightAccelX(float x);
    void setFlightAccelY(float y);
    void setFlightAccelZ(float z);
    void setFlightAltitude(float altitude);
    void setFlightAltitudeApogee(float altitude);
    void setFlightAltitudeApogeeFirstMeasure(float altitude);
    void setFlightAltitudeCurrent(float altitude);
    void setFlightAltitudeLast(float altitude);
    void setFlightAltitudeLaunch(float altitude);
    void setFlightAltitudeTouchdown(float altitude);
    void setFlightGyroX(float x);
    void setFlightGyroY(float y);
    void setFlightGyroZ(float z);
    void setFlightHumidity(float humidity);
    void setFlightPressure(float pressure);
    void setFlightTemperature(float temperature);
    void setFlightVelocity(float velocity);
    void setFlightTime(long diffTime);
    void setFlightTimestampApogee(long diffTime);
    void setFlightTimestampApogeeFirstMeasure(long diffTime);
    void setFlightTimestampCurrent(long diffTime);
    void setFlightTimestampLaunch(long diffTime);
    void setFlightTimestampPrevious(long diffTime);
    void setFlightTimestampTouchdown(long diffTime);
    bool writeFlight(int flightNbr);
    bool writeFlightFast();

  protected:
    flightDataTraceStruct _currentRecord;
    int _dataPos;
    flightDataStruct _flightData;
    flightDataTraceStruct* _flightDataTrace;
    flightMinAndMaxStruct _flightMinAndMax;
};

#endif