#ifndef _FLIGHT_LOGGER_BASE_H
#define _FLIGHT_LOGGER_BASE_H

#include <ArduinoJson.h>

#include "flightLoggerData.h"

class flightLoggerBase {
  public:
    bool clearFlights();
    bool copyTraceCurrentToArray();
    void determineTraceMinAndMax(int flightNbr);
    bool exists(int flightNbr);
    float getAccelXMax();
    float getAccelXMin();
    float getAccelYMax();
    float getAccelYMin();
    float getAccelZMax();
    float getAccelZMin();
    float getAltitudeMax();
    float getltitudeMin();
    flightDataStruct getData();
    flightDataTraceStruct* getDataTrace();
    long getDuration();
    float getHumidityMax();
    float getHumidityMin();
    long geFlightNbrLast();
    float getPressureMax();
    float getFlightPressureMin();
    long getFlightSize();
    float getTemperatureMax();
    float getTemperatureMin();
    float getVelocityMax();
    float getVelocityMin();
    void init(unsigned long timestamp);
    bool initFileSystem();
    void listAsJson(JsonArray flightLogs);
    void outputSerial();
    void outputSerial(int flightNbr);
    void outputSerialExpanded();
    void outputSerialExpanded(int flightNbr);
    void outputSerialList();
    bool readFile(int flightNbr);
    JsonObject readFileAsJson(int flightNbr);
    void reset();
    void setTraceCurrentAccelX(float x);
    void setTraceCurrentAccelY(float y);
    void setTraceCurrentAccelZ(float z);
    void setTraceCurrentAltitude(float altitude);
    void setAltitudeApogee(float altitude);
    void setAltitudeApogeeFirstMeasure(float altitude);
    void setAltitudeCurrent(float altitude);
    void setAltitudeLast(float altitude);
    void setAltitudeLaunch(float altitude);
    void setAltitudeTouchdown(float altitude);
    void setTraceCurrentGyroX(float x);
    void setTraceCurrentGyroY(float y);
    void setTraceCurrentGyroZ(float z);
    void setTraceCurrentHumidity(float humidity);
    void setTraceCurrentPressure(float pressure);
    void setTraceCurrentTemperature(float temperature);
    void setTraceCurrentVelocity(float velocity);
    void setTraceCurrentTime(long diffTime);
    void setTimestampApogee(long diffTime);
    void setTimestampApogeeFirstMeasure(long diffTime);
    void setTimestampCurrent(long diffTime);
    void setTimestampLaunch(long diffTime);
    void setTimestampPrevious(long diffTime);
    void setTimestampTouchdown(long diffTime);
    bool writeFile(int flightNbr);
    bool writeFlightCurrent();

  protected:
    flightDataStruct _flightData;
    flightDataTraceStruct* _flightDataTrace;
    flightDataTraceStruct _flightDataTraceCurrent;
    int _flightDataTraceIndex;
    flightMinAndMaxStruct _flightDataTraceMinMax;
};

#endif