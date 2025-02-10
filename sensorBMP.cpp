#include "debug.h"
#include "constants.h"
#include "sensor.h"
#include "sensorBMP.h"
#include "simulation.h"
#include "utilities.h"

sensorBMP::sensorBMP() {
}

atmosphereValues sensorBMP::initializeSensors() {
  atmosphereValues values;

  Serial.println(F("\tinitializeSensors..."));
  float resultHumidity = 0;
  float resultPressure = 0;
  float resultTemperature = 0;
  byte samples = 20;
  float sumHumidity = 0;
  float sumPressure = 0;
  float sumTemperature = 0;
  for (int i = 0; i < samples; i++) {
    debug(F("i"), i);
    resultHumidity = readSensor().humidity;
    resultPressure = readSensor().pressure;
    resultTemperature = readSensor().temperature;
    debug(F("resultHumidity"), resultHumidity);
    debug(F("resultPressure"), resultPressure);
    debug(F("resultTemperature"), resultTemperature);
    sumHumidity += resultHumidity;
    sumPressure += resultPressure;
    sumTemperature += resultTemperature;
    debug(F("sumHumidity"), sumHumidity);
    debug(F("sumPressure"), sumPressure);
    debug(F("sumTemperature"), sumTemperature);
    delay(50);
  }
  values.humidity = (sumHumidity / samples);
  values.pressure = (sumPressure / samples);
  values.temperature = (sumTemperature / samples);
  debug(F("atmosphereValues.humidity"), values.humidity);
  debug(F("atmosphereValues.pressure"), values.pressure);
  debug(F("temperatureOutdoor"), values.temperature);
  
  float result = 0;
  float sum = 0;
  for (int i = 0; i < samples; i++) {
    // debug(F("i"), i);
    result = readSensorAltitude();
    // debug(F("result"), result);
    sum += result;
    // debug(F("sum"), sum);
    delay(50);
  }
  float altitudeInitial = (sum / samples);
  debug(F("altitudeInitial"), altitudeInitial);
  Serial.println(F("\t...initializeSensors"));
  values.altitude = altitudeInitial;
  return values;
}

atmosphereValues sensorBMP::readSensor() {
  atmosphereValues values;

  uint32_t pressure = _sensor.getPressure();
  float temperature = _sensor.getTemperature();
  // values.humidity = humidity;
  values.pressure = (float)pressure / 100.0;
  values.temperature = temperature;

#if defined(DEBUG_SENSOR)
  Serial.print(F("refPres="));
  Serial.print(pressureReference);
  Serial.print(F("humidity="));
  Serial.print(humidity);
  Serial.print(F("\t"));
  Serial.print(F("pressure="));
  Serial.print(pressure);
  Serial.print(F("\t"));
  Serial.print(F("temperature="));
  Serial.print(temperature);
  Serial.print(F("\t"));
#endif

  return values;
}

float sensorBMP::readSensorAltitude() {
  atmosphereValues values = readSensor();
  return readSensorAltitude(values);
}

float sensorBMP::readSensorAltitude(atmosphereValues values) {
  float pressure = values.pressure * 100;

#if defined(KALMAN) && defined(KALMAN_ALTITUDE)
  float pressureK = _kalmanPressure.kalmanCalc(altitude);
#if defined(DEBUG_SENSOR)
  Serial.print(F("__kalmanPressure="));
  Serial.println(pressureK);
#endif
  pressure = pressureK;
#endif

  // float altitude = _sensor.calAltitude(pressure, pressureReference);
  float altitude = 44330 * (1.0 - pow(pressure / 100.0 / pressureReference, 0.1903));
#if defined(DEBUG_SENSOR)
  Serial.print(F("\tpressure="));
  Serial.println(values.pressure);
  Serial.print(F("\taltitude="));
  Serial.println(altitude);
#endif
  values.altitude = altitude;

//   // alternate to compare...
//   float altitude2 = NAN;
//   if (!isnan(values.pressure) && !isnan(pressureReference) && !isnan(temperatureOutdoor))
//   {
//       altitude2 = pow(pressureReference / values.pressure, 0.190234) - 1;
//       altitude2 *= ((temperatureOutdoor + 273.15) / 0.0065);
//   }
//   values.altitude = altitude2;
// #if defined(DEBUG_SENSOR)
//   Serial.print(F("altitude2="));
//   Serial.println(altitude2);
// #endif

#ifdef DEV_SIM
  if (_simulation.isRunning() && _initialized) { 
    // values.altitude = simulationValueAltitude();
    values.altitude = _simulation.valueAltitude();
    debug("sim.altitude", values.altitude);
  }
#endif

#if defined(KALMAN) && defined(KALMAN_ALTITUDE)
  float altitudeK = _kalmanAltitude.kalmanCalc(altitude);
#if defined(DEBUG_SENSOR)
  Serial.print(F("_kalmanAltitude="));
  Serial.println(altitudeK);
#endif
  values.altitude = altitudeK;
#endif

  return values.altitude;
}

void sensorBMP::sleepSensors() {
  Serial.println(F("\tSleep sensor atmosphere..."));

  Serial.println(F("\t...sensor atmosphere sleep successful."));
}

void sensorBMP::setupSensors() {
  Serial.println(F("\tSetup sensor atmosphere..."));

  _sensor.begin();

  Serial.println(F("\t...BME280 initialized."));

  Serial.println(F("\t\tSetup Kalman filter..."));

  // let's do some dummy altitude reading to initialise the Kalman filter
#if defined(KALMAN)
  for (int i = 0; i < 50; i++) {
// #ifdef DEBUG
//   Serial.print(F("Reading atmosphere...");
//   Serial.println(i);
// #endif
//   readSensor();
  }
#endif

  Serial.println(F("\t\t...Kalman filter initialized."));

  Serial.println(F("\t...sensor atmosphere successful."));
}