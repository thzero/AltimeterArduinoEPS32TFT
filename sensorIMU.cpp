#include "SensorQMI8658.hpp"

#include "debug.h"
#include "constants.h"
#include "kalman.h"
#include "sensor.h"
#include "sensorIMU.h"
#include "simulation.h"
#include "utilities.h"

sensorIMU::sensorIMU() {
}

accelerometerValues sensorIMU::readSensorAccelerometer() {
  accelerometerValues values;
  if (_qmi.getDataReady()) {
    if (_qmi.getAccelerometer(_qmiData.acc.x, _qmiData.acc.y, _qmiData.acc.z)) {
        values.x = _qmiData.acc.x;
        values.y = _qmiData.acc.y;
        values.z = _qmiData.acc.x;
      }
    }

#ifdef DEV_SIM
  if (_simulation.isRunning()) { 
    values.x = 0; // TODO: Simulation currently does not have these values.
    values.y = 0; // TODO: Simulation currently does not have these values.
    values.z = _simulation.valueAltitude();
  }
#endif

#if defined(KALMAN) && defined(KALMAN_ACCEL)
        float value = _kalmanAccelX.kalmanCalc(values.x);
  #if defined(DEBUG_SENSOR)
        Serial.print(F("_kalmanAccelX="));
        Serial.println(value);
  #endif
        values.x = value;

        value = _kalmanAccelY.kalmanCalc(values.y);
  #if defined(DEBUG_SENSOR)
        Serial.print(F("_kalmanAccelY="));
        Serial.println(value);
  #endif
        values.y = value;
        
        value = _kalmanAccelZ.kalmanCalc(values.z);
  #if defined(DEBUG_SENSOR)
        Serial.print(F("_kalmanAccelZ="));
        Serial.println(value);
  #endif
        values.z = value;
#endif

#if defined(DEBUG_SENSOR)
  Serial.print(F("accelerometer.x="));
  Serial.print(values.accX);
  Serial.print(F("\t"));
  Serial.print(F("accelerometer.y="));
  Serial.print(values.accY);
  Serial.print(F("\t"));
  Serial.print(F("accelerometer.z="));
  Serial.print(values.accZ);
#endif

    return values;
}

gyroscopeValues sensorIMU::readSensorGyroscope() {
  gyroscopeValues values;
  if (_qmi.getDataReady()) {
    if (_qmi.getGyroscope(_qmiData.gyr.x, _qmiData.gyr.y, _qmiData.gyr.z)) {
        values.x = (float)_qmiData.gyr.x;
        values.y = (float)_qmiData.gyr.y;
        values.z = (float)_qmiData.gyr.x;
      }
    }

#ifdef DEV_SIM
  if (_simulation.isRunning()) { 
    // TODO: Simulation currently does not have these values.
    values.x = 0;
    values.y = 0;
    values.z = 0;
  }
#endif

#if defined(KALMAN) && defined(KALMAN_ACCEL)
        float value = _kalmanGyroX.kalmanCalc(values.x);
  #if defined(DEBUG_SENSOR)
        Serial.print(F("_kalmanGyroX="));
        Serial.println(value);
  #endif
        values.x = value;

        value = _kalmanGyroY.kalmanCalc(values.y);
  #if defined(DEBUG_SENSOR)
        Serial.print(F("_kalmanGyroY="));
        Serial.println(value);
  #endif
        values.y = value;
        
        value = _kalmanGyroZ.kalmanCalc(values.z);
  #if defined(DEBUG_SENSOR)
        Serial.print(F("_kalmanGyroZ="));
        Serial.println(value);
  #endif
        values.z = value;
#endif

#if defined(DEBUG_SENSOR)
  Serial.print(F("gyro.x="));
  Serial.print(values.x);
  Serial.print(F("\t"));
  Serial.print(F("gyro.y="));
  Serial.print(values.y);
  Serial.print(F("\t"));
  Serial.print(F("gyro.z="));
  Serial.print(values.z);
#endif

    return values;
}

void sensorIMU::sleepSensors() {
  Serial.println(F("\tSleep sensor IMU..."));
  
  _qmi.disableGyroscope();
  _qmi.disableAccelerometer();

  Serial.println(F("\t...sensor IMU sleep successful."));
}

void sensorIMU::setupSensors() {
  Serial.println(F("\tSetup sensor IMU..."));
  
  // Initialize QMI8658C sensor with provided configuration
  while (!_qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL)) {
    // _tft.drawString("QMI8658C error", 6, 195); // todo
    delay(500);
  }
  delay(1000); // Delay for sensor initialization

  Serial.println(F("\t\tSetup sensor IMU...config..."));
  _qmi.configAccelerometer(
    /*
       ACC_RANGE_2G
       ACC_RANGE_4G
       ACC_RANGE_8G
       ACC_RANGE_16G
    */

    /*
    // TODO
    The level of spikes (at least on my board) is proportional to the ODR that I configure for the IMU. If I set the ODR to 235Hz, 
    I get a signal with no spikes at all. At 940Hz, I get what I showed. And at full tilt (7520 Hz), it looks like a porcupine. 
    Enabling the low-pass filter certainly helps, but it doesn't entirely get rid of the spikes, like setting a lower sample rate does. 
    So, at least for my setup where I can't sample much faster than 250Hz anyway, I'm going to stick to an ODR of 235Hz.
    */
    SensorQMI8658::ACC_RANGE_4G,
    /*
       ACC_ODR_1000Hz
       ACC_ODR_500Hz
       ACC_ODR_250Hz
       ACC_ODR_125Hz
       ACC_ODR_62_5Hz
       ACC_ODR_31_25Hz
       ACC_ODR_LOWPOWER_128Hz
       ACC_ODR_LOWPOWER_21Hz
       ACC_ODR_LOWPOWER_11Hz
       ACC_ODR_LOWPOWER_3H
    */
    SensorQMI8658::ACC_ODR_1000Hz,
    /*
       LPF_MODE_0     //2.66% of ODR
       LPF_MODE_1     //3.63% of ODR
       LPF_MODE_2     //5.39% of ODR
       LPF_MODE_3     //13.37% of ODR
       LPF_OFF        // OFF Low-Pass Fitter
    */
    SensorQMI8658::LPF_MODE_0);

  _qmi.configGyroscope(
    /*
      GYR_RANGE_16DPS
      GYR_RANGE_32DPS
      GYR_RANGE_64DPS
      GYR_RANGE_128DPS
      GYR_RANGE_256DPS
      GYR_RANGE_512DPS
      GYR_RANGE_1024DPS
    */
    SensorQMI8658::GYR_RANGE_64DPS,
    /*
       GYR_ODR_7174_4Hz
       GYR_ODR_3587_2Hz
       GYR_ODR_1793_6Hz
       GYR_ODR_896_8Hz
       GYR_ODR_448_4Hz
       GYR_ODR_224_2Hz
       GYR_ODR_112_1Hz
       GYR_ODR_56_05Hz
       GYR_ODR_28_025H
    */
    SensorQMI8658::GYR_ODR_896_8Hz,
    /*
       LPF_MODE_0     //2.66% of ODR
       LPF_MODE_1     //3.63% of ODR
       LPF_MODE_2     //5.39% of ODR
       LPF_MODE_3     //13.37% of ODR
       LPF_OFF        // OFF Low-Pass Fitter
    */
    SensorQMI8658::LPF_MODE_3);

  /*
    If both the accelerometer and gyroscope sensors are turned on at the same time,
    the output frequency will be based on the gyroscope output frequency.
    The example configuration is 896.8HZ output frequency,
    so the acceleration output frequency is also limited to 896.8HZ
  */
  _qmi.enableGyroscope();
  _qmi.enableAccelerometer();
  Serial.println(F("\t\t...sensor IMU config successful."));

  Serial.println(F("\t...sensor IMU successful."));
}