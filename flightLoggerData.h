#ifndef _FLIGHT_LOGGER_DATA_H
#define _FLIGHT_LOGGER_DATA_H

struct flightLoggerData {
  bool airborne;
  float altitudeApogee;
  float altitudeApogeeFirstMeasure;
  float altitudeCurrent;
  float altitudeInitial;
  float altitudeLast;
  float altitudeLaunch;
  float altitudeTouchdown;
  // Number of measures to do so that we are sure that apogee has been reached
  unsigned long measures = 5;
  float pressureInitial;
  float temperatureInitial;
  bool touchdown;
  unsigned long timestampLaunch = 0;
  unsigned long timestampCurrent = 0;
  unsigned long timestampPrevious = 0;
  unsigned long timestampApogee = 0;
  unsigned long timestampApogeeFirstMeasure = 0;
  unsigned long timestampTouchdown = 0;
};

struct flightDataStruct {
  float accelX;
  float accelY;
  float accelZ;
  long altitude;
  long diffTime;
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

#endif