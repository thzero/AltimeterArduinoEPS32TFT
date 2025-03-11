#include "debug.h"
#include "constants.h"
#include "sensor.h"
// #include "sensorBME.h"
#include "sensorBMP.h"
#include "sensorIMU.h"

// sensorBME _atmosphereSensor;
sensorBMP _atmosphereSensor;
sensorIMU _imuSensor;

atmosphereValues initializeSensorsAtmosphere() {
  return _atmosphereSensor.initializeSensors();
}

atmosphereValues readSensorAtmosphere() {
  return _atmosphereSensor.readSensor();
}

float readSensorAltitude() {
  return _atmosphereSensor.readSensorAltitude();
}

float readSensorAltitude(atmosphereValues values) {
  return _atmosphereSensor.readSensorAltitude(values);
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

  _atmosphereSensor.sleepSensors();
  
  _imuSensor.sleepSensors();

  Serial.println(F("...sensors sleep successful."));
}

void setupSensors() {
  if (_initialized)
    return;

  stack_t tracer;
  Serial.println(F("\nSetup sensors..."));
  _initialized = false;

  _atmosphereSensor.setupSensors();
  
  _imuSensor.setupSensors();

  Serial.println(F("...sensors successful."));
}

void setupSensorsCompleted() {
  Serial.println(F("\nSetup sensors completed..."));
  _initialized = true;
  Serial.println(F("...sensors completed successful."));
}