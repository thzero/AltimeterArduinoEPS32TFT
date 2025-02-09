#include <BME280I2C.h>
// #include <BME280_Arduino_I2C.h>
// #include "BME280_Arduino_I2C.h"
#include <EnvironmentCalculations.h>

#include "debug.h"
#include "constants.h"
#include "sensor.h"
#include "sensorBME.h"
#include "simulation.h"
#include "utilities.h"

BME280I2C::Settings bmeSettings(
  BME280::OSR_X1,
  BME280::OSR_X1,
  BME280::OSR_X1,
  BME280::Mode_Forced,
  BME280::StandbyTime_1000ms,
  BME280::Filter_Off,
  BME280::SpiEnable_False,
  BME280I2C::I2CAddr_0x77 // I2C address. I2C specific.
);
// BME280I2C _bme(bmeSettings);

sensorBME::sensorBME() {
  _bme = BME280I2C(bmeSettings);
  // _bme = BME280_Arduino_I2C(0x77);
}

atmosphereValues sensorBME::initializeSensors() {
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

atmosphereValues sensorBME::readSensor() {
  atmosphereValues values;

  float temperature(NAN), humidity(NAN), pressure(NAN);

  BME280::TempUnit temperatureUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit pressureUnit(BME280::PresUnit_hPa);

  _bme.read(pressure, temperature, humidity, temperatureUnit, pressureUnit);
  values.humidity = humidity;
  values.pressure = pressure;
  values.temperature = temperature;

#if defined(DEBUG_SENSOR)
  Serial.print(F("refPres="));
  Serial.print(pressureReference);
  Serial.print(F("\t"));
  Serial.print(F("pressure="));
  Serial.print(pressure);
  Serial.print(F("\t"));
  Serial.print(F("temperature="));
  Serial.print(temperature);
  Serial.print(F("\t"));
  Serial.print(F("humidity="));
  Serial.print(humidity);
#endif

  return values;
}

float sensorBME::readSensorAltitude() {
  atmosphereValues values = readSensor();
  return readSensorAltitude(values);
}

float sensorBME::readSensorAltitude(atmosphereValues values) {
  EnvironmentCalculations::AltitudeUnit envAltUnit = EnvironmentCalculations::AltitudeUnit_Meters;
  EnvironmentCalculations::TempUnit envTempUnit = EnvironmentCalculations::TempUnit_Celsius;

  float pressure = values.pressure;

#if defined(KALMAN) && defined(KALMAN_ALTITUDE)
  float pressureK = _kalmanPressure.kalmanCalc(altitude);
#if defined(DEBUG_SENSOR)
  Serial.print(F("__kalmanPressure="));
  Serial.println(pressureK);
#endif
  pressure = pressureK;
#endif

   // TODO: This really should be based on the temperature that at this altitude... using a static temp reduces precision
  float altitude = EnvironmentCalculations::Altitude(pressure, envAltUnit, pressureReference, temperatureOutdoor, envTempUnit);
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

void sensorBME::sleepSensors() {
  Serial.println(F("\tSleep sensor atmosphere..."));

  Serial.println(F("\t...sensor atmosphere sleep successful."));
}

void sensorBME::setupSensors() {
  Serial.println(F("\tSetup sensor atmosphere..."));

  bool results = _bme.begin();
  if (!results) {
    // _tft.drawString("BME280 error", 6, 195); // TODO
    Serial.println(F("\t\tBME280 error")); // TODO
    delay(500);
  
    Serial.println(F("\t...sensor atmosphere failed."));
    return;
  }

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