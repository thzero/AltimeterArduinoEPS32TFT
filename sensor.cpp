#include "debug.h"
#include "constants.h"
#include "sensor.h"
#include "sensorBME.h"
#include "sensorIMU.h"

sensorBME _bmeSensor;
sensorIMU _imuSensor;

atmosphereValues initializeSensorsAtmosphere() {
  return _bmeSensor.initializeSensors();
}

atmosphereValues readSensorAtmosphere() {
  return _bmeSensor.readSensor();
}

float readSensorAltitude() {
  return _bmeSensor.readSensorAltitude();
}

float readSensorAltitude(atmosphereValues values) {
  return _bmeSensor.readSensorAltitude(values);
}

accelerometerValues readSensorAccelerometer() {
  return _imuSensor.readSensorAccelerometer();
}

gyroscopeValues readSensorGyroscope() {
  return _imuSensor.readSensorGyroscope();
}

void sleepSensors() {
  if (_initialized)
    return;

  Serial.println(F("Sleep sensors..."));

  _bmeSensor.sleepSensors();
  
  _imuSensor.sleepSensors();

  Serial.println(F("...sensors sleep successful."));
}

void setupSensors() {
  if (_initialized)
    return;

  stack_t tracer;
  Serial.println(F("Setup sensors..."));
  _initialized = false;

  _bmeSensor.setupSensors();
  
  _imuSensor.setupSensors();

  Serial.println(F("...sensors successful."));
}

void setupSensorsCompleted() {
  Serial.println(F("Setup sensors completed..."));
  _initialized = true;
  Serial.println(F("...sensors completed successful."));
}