#ifndef _SENSOR_H
#define _SENSOR_H

#include "sensorData.h"

extern atmosphereValues initializeSensorsAtmosphere();

extern float readSensorAltitude();
extern float readSensorAltitude(atmosphereValues values);
extern atmosphereValues readSensorAtmosphere();
extern accelerometerValues readSensorAccelerometer();
extern gyroscopeValues readSensorGyroscope();

extern void setupSensors();
extern void setupSensorsCompleted();

#endif