#ifndef _FLIGHT_LOGGER_DATA_H
#define _FLIGHT_LOGGER_DATA_H

#include <ArduinoJson.h>

struct flightDataStruct {
  float altitudeApogee;
  float altitudeApogeeFirstMeasure;
  float altitudeCurrent;
  float altitudeInitial;
  float altitudeLast;
  float altitudeLaunch;
  float altitudeTouchdown;
  unsigned long epochS = 0;
  unsigned long timestampApogee = 0;
  unsigned long timestampApogeeFirstMeasure = 0;
  unsigned long timestampCurrent = 0;
  unsigned long timestampLaunch = 0;
  unsigned long timestampPrevious = 0;
  unsigned long timestampTouchdown = 0;
};

struct flightDataReadResultsStruct {
    bool success;
    JsonObject results;
};

struct flightDataTraceStruct {
  float accelX;
  float accelY;
  float accelZ;
  long altitude;
  long diffTime;
  float gyroX;
  float gyroY;
  float gyroZ;
  float humidity;
  float pressure;
  float temperature;
  float velocity;
};

struct flightMinAndMaxStruct {
  long duration;
  float accelXMax;
  float accelXMin;
  float accelYMin;
  float accelYMax;
  float accelZMin;
  float accelZMax;
  long altitudeMax;
  long altitudeMin;
  long humidityMax;
  long humidityMin;
  long pressureMax;
  long pressureMin;
  long temperatureMax;
  long temperatureMin;
  long velocityMax;
  long velocityMin;
};

struct flightDataNumberStruct {
    int * numbers;
    int size;
};

#endif