#ifndef _SENSOR_IMU_H
#define _SENSOR_IMU_H

#include "SensorQMI8658.hpp"

#include "sensorData.h"

//////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////
#ifndef SENSOR_SDA
#define SENSOR_SDA 42
#endif

#ifndef SENSOR_SCL
#define SENSOR_SCL 41
#endif

struct qmiData {
  SensorQMI8658 instance;
  IMUdata acc;
  IMUdata gyr;
};

class sensorIMU {
  public:
    sensorIMU();
    void sleepSensors();
    void setupSensors();
    accelerometerValues readSensorAccelerometer();
    gyroscopeValues readSensorGyroscope();
    
  private:
    struct qmiData _qmiData;
    SensorQMI8658 _qmi;
};

#endif