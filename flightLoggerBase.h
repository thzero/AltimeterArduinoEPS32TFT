#ifndef _FLIGHT_LOGGER_BASE_H
#define _FLIGHT_LOGGER_BASE_H

#include "flightLoggerData.h"

class flightLoggerBase {
  public:
    virtual bool clearFlightList();
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
    virtual flightDataStruct* getFlightData();
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
    virtual bool initFlight();
    virtual void printFlightData(int flightNbr);
    virtual bool readFlight(int flightNbr);
    virtual void setFlightAccelX(float x);
    virtual void setFlightAccelY(float y);
    virtual void setFlightAccelZ(float z);
    virtual void setFlightAltitude(float altitude);
    virtual bool setFlightData();
    virtual void setFlightGyroX(float x);
    virtual void setFlightGyroY(float y);
    virtual void setFlightGyroZ(float z);
    virtual void setFlightHumidity(float humidity);
    virtual void setFlightPressure(float pressure);
    virtual void setFlightTemperature(float temperature);
    virtual void setFlightVelocity(float velocity);
    virtual void setFlightTime(long diffTime);
    virtual bool writeFlight(int flightNbr);
    virtual bool writeFlightFast();

  protected:
    flightDataStruct _currentRecord;
    int _dataPos;
    flightDataStruct* _flightData;
    flightMinAndMaxStruct _flightMinAndMax;
};

#endif