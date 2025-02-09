#ifndef _SENSOR_BME_H
#define _SENSOR_BME_H

#include <BME280I2C.h>
// #include <BME280_Arduino_I2C.h>
// #include "BME280_Arduino_I2C.h"

#include "kalman.h"
#include "sensorData.h"

// extern BME280I2C::Settings bmeSettings;

class sensorBME {
  public:
    sensorBME();
    // void init(BME280I2C bme);
    atmosphereValues initializeSensors();
    void initSensors();
    atmosphereValues readSensor();
    float readSensorAltitude();
    float readSensorAltitude(atmosphereValues values);
    void sleepSensors();
    void setupSensors();
    
  private:
    BME280I2C _bme;
    // BME280_Arduino_I2C _bme;
    int _count;
    kalman _kalmanAltitude;
    kalman _kalmanPressure;
};

#endif