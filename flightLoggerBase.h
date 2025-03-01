#ifndef _FLIGHT_LOGGER_BASE_H
#define _FLIGHT_LOGGER_BASE_H

#include "flightLoggerData.h"

class flightLoggerBase {
  public:
    virtual bool clearFlightList();
    virtual bool copyFlightDataTrace();
    virtual long getFlightDuration();
    virtual void determineFlightMinAndMax(int flightNbr);
    virtual float getFlightAccelXMax();
    virtual float getFlightAccelXMin();
    virtual float getFlightAccelYMax();
    virtual float getFlightAccelYMin();
    virtual float getFlightAccelZMax();
    virtual float getFlightAccelZMin();
    virtual float getFlightAltitudeMax();
    virtual float getFlightAltitudeMin();
    virtual flightDataStruct getFlightData();
    virtual flightDataTraceStruct* getFlightDataTrace();
    virtual float getFlightHumidityMax();
    virtual float getFlightHumidityMin();
    virtual float getFlightPressureMax();
    virtual float getFlightPressureMin();
    virtual float getFlightTemperatureMax();
    virtual float getFlightTemperatureMin();
    virtual float getFlightVelocityMax();
    virtual float getFlightVelocityMin();
    virtual long geFlightNbrLast();
    virtual long getFlightSize();
    virtual bool initFileSystem();
    virtual void initFlight(unsigned long timestamp);
    virtual void printFlightData(int flightNbr);
    virtual bool readFlight(int flightNbr);
    virtual void resetFlight();
    virtual void setFlightAccelX(float x);
    virtual void setFlightAccelY(float y);
    virtual void setFlightAccelZ(float z);
    virtual void setFlightAltitude(float altitude);
    virtual void setFlightAltitudeApogee(float altitude);
    virtual void setFlightAltitudeApogeeFirstMeasure(float altitude);
    virtual void setFlightAltitudeCurrent(float altitude);
    virtual void setFlightAltitudeLast(float altitude);
    virtual void setFlightAltitudeLaunch(float altitude);
    virtual void setFlightAltitudeTouchdown(float altitude);
    virtual void setFlightGyroX(float x);
    virtual void setFlightGyroY(float y);
    virtual void setFlightGyroZ(float z);
    virtual void setFlightHumidity(float humidity);
    virtual void setFlightPressure(float pressure);
    virtual void setFlightTemperature(float temperature);
    virtual void setFlightVelocity(float velocity);
    virtual void setFlightTime(long diffTime);
    virtual void setFlightTimestampApogee(long diffTime);
    virtual void setFlightTimestampApogeeFirstMeasure(long diffTime);
    virtual void setFlightTimestampCurrent(long diffTime);
    virtual void setFlightTimestampLaunch(long diffTime);
    virtual void setFlightTimestampPrevious(long diffTime);
    virtual void setFlightTimestampTouchdown(long diffTime);
    virtual bool writeFlight(int flightNbr);
    virtual bool writeFlightFast();

  protected:
    flightDataTraceStruct _currentRecord;
    int _dataPos;
    flightDataStruct _flightData;
    flightDataTraceStruct* _flightDataTrace;
    flightMinAndMaxStruct _flightMinAndMax;
};

#endif