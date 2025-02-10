#ifndef _SENSOR_BME_H
#define _SENSOR_BME_H

#include <BMP280.h>

#include "kalman.h"
#include "sensorData.h"

class sensorBMP {
  public:
    sensorBMP();
    // void init(BME280I2C bme);
    atmosphereValues initializeSensors();
    void initSensors();
    atmosphereValues readSensor();
    float readSensorAltitude();
    float readSensorAltitude(atmosphereValues values);
    void sleepSensors();
    void setupSensors();
    
  private:
    BMP280 _sensor;
    int _count;
    kalman _kalmanAltitude;
    kalman _kalmanPressure;
};

#endif