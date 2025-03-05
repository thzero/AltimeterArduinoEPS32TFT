#include <Arduino.h>

#include "constants.h"
#include "flightLoggerBase.h"
#include "time.h"
#include "utilities.h"

bool flightLoggerBase::clearFlightList() {
  return false;
}

// Add to the _currentRecord to the _flightDataTrace array
bool flightLoggerBase::copyFlightDataTrace() {
  flightDataTraceStruct* newFlightData = (flightDataTraceStruct*)realloc(_flightDataTrace, (_dataPos + 1) * sizeof(flightDataTraceStruct));
  if (!newFlightData)
    return false;

  _flightDataTrace = newFlightData;
  _flightDataTrace[_dataPos] = _currentRecord;
  _dataPos++;
  return true;
}

void flightLoggerBase::determineFlightMinAndMax(int flightNbr) {
  _flightMinAndMax.accelXMax = 0;
  _flightMinAndMax.accelXMin = 0;
  _flightMinAndMax.accelYMax = 0;
  _flightMinAndMax.accelYMin = 0;
  _flightMinAndMax.accelZMax = 0;
  _flightMinAndMax.accelZMin = 0;
  _flightMinAndMax.accelZMin = 0;
  _flightMinAndMax.altitudeMax = 0;
  _flightMinAndMax.duration = 0;
  _flightMinAndMax.pressureMax = 0;
  _flightMinAndMax.pressureMin = 0;
  _flightMinAndMax.temperatureMax = 0;
  _flightMinAndMax.temperatureMin = 0;
  _flightMinAndMax.velocityMax = 0;
  _flightMinAndMax.velocityMin = 0;

  if (readFlight(flightNbr)) {
    for (int i = 0; i < _dataPos ; i++)
    {
      _flightMinAndMax.duration = _flightMinAndMax.duration + _flightDataTrace[i].diffTime;

      if (_flightDataTrace[i].accelX < _flightMinAndMax.accelXMin)
        _flightMinAndMax.accelXMin = _flightDataTrace[i].accelX;
      if (_flightDataTrace[i].accelX > _flightMinAndMax.accelXMax)
        _flightMinAndMax.accelXMax = _flightDataTrace[i].accelX;

      if (_flightDataTrace[i].accelY < _flightMinAndMax.accelYMin)
        _flightMinAndMax.accelXMin = _flightDataTrace[i].accelY;
      if (_flightDataTrace[i].accelY > _flightMinAndMax.accelYMax)
        _flightMinAndMax.accelXMax = _flightDataTrace[i].accelY;

      if (_flightDataTrace[i].accelZ < _flightMinAndMax.accelZMin)
        _flightMinAndMax.accelZMin = _flightDataTrace[i].accelZ;
      if (_flightDataTrace[i].accelZ > _flightMinAndMax.accelZMax)
        _flightMinAndMax.accelZMax = _flightDataTrace[i].accelZ;

      if (_flightDataTrace[i].altitude < _flightMinAndMax.altitudeMin)
        _flightMinAndMax.altitudeMin = _flightDataTrace[i].altitude;
      if (_flightDataTrace[i].altitude > _flightMinAndMax.altitudeMax)
        _flightMinAndMax.altitudeMax = _flightDataTrace[i].altitude;

      if (_flightDataTrace[i].pressure < _flightMinAndMax.pressureMin)
        _flightMinAndMax.pressureMin = _flightDataTrace[i].pressure;
      if (_flightDataTrace[i].pressure > _flightMinAndMax.temperatureMax)
        _flightMinAndMax.pressureMax = _flightDataTrace[i].pressure;

      if (_flightDataTrace[i].temperature < _flightMinAndMax.temperatureMin)
        _flightMinAndMax.temperatureMin = _flightDataTrace[i].temperature;
      if (_flightDataTrace[i].temperature > _flightMinAndMax.temperatureMax)
        _flightMinAndMax.temperatureMax = _flightDataTrace[i].temperature;

      if (_flightDataTrace[i].velocity < _flightMinAndMax.velocityMin)
        _flightMinAndMax.temperatureMin = _flightDataTrace[i].velocity;
      if (_flightDataTrace[i].velocity > _flightMinAndMax.velocityMax)
        _flightMinAndMax.velocityMax = _flightDataTrace[i].velocity;
    }
  }
}

bool flightLoggerBase::existsFlight(int flightNbr) {
  return false;
}

float flightLoggerBase::getFlightAccelXMax() {
  return _flightMinAndMax.accelXMax;
}

float flightLoggerBase::getFlightAccelXMin() {
  return _flightMinAndMax.accelXMin;
}

float flightLoggerBase::getFlightAccelYMax() {
  return _flightMinAndMax.accelYMax;
}

float flightLoggerBase::getFlightAccelYMin() {
  return _flightMinAndMax.accelYMin;
}

float flightLoggerBase::getFlightAccelZMax() {
  return _flightMinAndMax.accelZMax;
}

float flightLoggerBase::getFlightAccelZMin() {
  return _flightMinAndMax.accelZMin;
}

float flightLoggerBase::getFlightAltitudeMax() {
  return _flightMinAndMax.altitudeMax;
}

float flightLoggerBase::getFlightAltitudeMin() {
  return _flightMinAndMax.altitudeMin;
}

flightDataStruct flightLoggerBase::getFlightData() {
  return _flightData;
}

flightDataTraceStruct* flightLoggerBase::getFlightDataTrace() {
  return _flightDataTrace;
}

long flightLoggerBase::getFlightDuration() {
  return _flightMinAndMax.duration;
}

float flightLoggerBase::getFlightHumidityMax() {
  return _flightMinAndMax.humidityMax;
}

float flightLoggerBase::getFlightHumidityMin() {
  return _flightMinAndMax.humidityMin;
}

long flightLoggerBase::geFlightNbrLast() {
  return _dataPos;
}

float flightLoggerBase::getFlightPressureMax() {
  return _flightMinAndMax.pressureMax;
}

float flightLoggerBase::getFlightPressureMin() {
  return _flightMinAndMax.pressureMin;
}

long flightLoggerBase::getFlightSize() {
  return _dataPos;
}

float flightLoggerBase::getFlightTemperatureMax() {
  return _flightMinAndMax.temperatureMax;
}

float flightLoggerBase::getFlightTemperatureMin() {
  return _flightMinAndMax.temperatureMin;
}

float flightLoggerBase::getFlightVelocityMax() {
  return _flightMinAndMax.velocityMax;
}

float flightLoggerBase::getFlightVelocityMin() {
  return _flightMinAndMax.velocityMin;
}

bool flightLoggerBase::initFileSystem() {
  return false;
}

void flightLoggerBase::initFlight(unsigned long timestamp) {
  resetFlight();
  _flightData.epochS = getEpoch();
  _flightData.timestampLaunch = timestamp;
}

void flightLoggerBase::printFlightData(int flightNbr) {
  unsigned long currentTime = 0;

  if (!readFlight(flightNbr))
    return;

  char format[4] = "%i,";
  char formatF[6] = "%.2f,";

  Serial.print(F("$"));
  Serial.print(_flightData.epochS);
  Serial.print(F("$"));
  Serial.print(_flightData.altitudeApogee);
  Serial.print(F("$"));
  Serial.print(_flightData.altitudeInitial);
  Serial.print(F("$"));
  Serial.print(_flightData.altitudeLaunch);
  Serial.print(F("$"));
  Serial.print(_flightData.altitudeTouchdown);
  Serial.print(F("$"));
  Serial.print(_flightData.timestampLaunch);
  Serial.print(F("$"));
  Serial.print(_flightData.timestampApogee);
  Serial.print(F("$"));
  Serial.print(_flightData.timestampTouchdown);

  for (int i = 0; i < _dataPos ; i++)
  {
    char flightDt[120] = "";
    char temp[20] = "";
    currentTime = currentTime + _flightDataTrace[i].diffTime;
    strcat(flightDt, "data,");
    sprintf(temp, format, currentTime);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightDataTrace[i].accelX * 1000);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightDataTrace[i].accelY * 1000);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightDataTrace[i].accelZ * 1000);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightDataTrace[i].altitude);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightDataTrace[i].gyroX * 1000);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightDataTrace[i].gyroY * 1000);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightDataTrace[i].gyroZ * 1000);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightDataTrace[i].humidity);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightDataTrace[i].pressure);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightDataTrace[i].temperature);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightDataTrace[i].velocity);
    strcat(flightDt, temp);

    unsigned int chk = msgChk(flightDt, sizeof(flightDt));
    sprintf(temp, "%i", chk);
    strcat(flightDt, temp);
    strcat(flightDt, ";\n");

    Serial.print(F("$"));
    Serial.print(flightDt);
  }
}

void flightLoggerBase::resetFlight() {
  _flightData.altitudeApogee = 0;
  _flightData.altitudeApogeeFirstMeasure = 0;
  _flightData.altitudeCurrent = 0;
  _flightData.altitudeLast = 0;
  _flightData.altitudeLaunch = 0;
  _flightData.altitudeTouchdown = 0;
  _flightData.epochS = 0;
  _flightData.timestampApogee = 0;
  _flightData.timestampApogeeFirstMeasure = 0;
  _flightData.timestampLaunch = 0;
  _flightData.timestampPrevious = 0;
  _flightData.timestampTouchdown = 0;
  
  if (_flightDataTrace != nullptr)
    free(_flightDataTrace);

  _flightDataTrace = nullptr;
  _dataPos = 0;
}

bool flightLoggerBase::readFlight(int flightNbr) {
  return false;
}

JsonObject flightLoggerBase::readFlightAsJson(int flightNbr) {
  DynamicJsonDocument doc(256);
  return doc.as<JsonObject>();
}

void flightLoggerBase::readFlightsAsJson(JsonArray flightLogs) {
  #ifdef DEBUG
    Serial.println(F("\tflightLoggerLFS.readFlightsAsJson..."));
  #endif
}

void flightLoggerBase::setFlightAccelX(float x) {
  _currentRecord.accelX = x;
}

void flightLoggerBase::setFlightAccelY(float y) {
  _currentRecord.accelY = y;
}

void flightLoggerBase::setFlightAccelZ(float z) {
  _currentRecord.accelZ = z;
}

void flightLoggerBase::setFlightAltitude(float altitude) {
  _currentRecord.altitude = altitude;
}

void flightLoggerBase::setFlightAltitudeApogee(float altitude) {
  _flightData.altitudeApogee = altitude;
}

void flightLoggerBase::setFlightAltitudeApogeeFirstMeasure(float altitude) {
  _flightData.altitudeApogeeFirstMeasure = altitude;
}

void flightLoggerBase::setFlightAltitudeCurrent(float altitude) {
  _flightData.altitudeCurrent = altitude;
}

void flightLoggerBase::setFlightAltitudeLast(float altitude) {
  _flightData.altitudeLast = altitude;
}

void flightLoggerBase::setFlightAltitudeLaunch(float altitude) {
  _flightData.altitudeLaunch = altitude;
}

void flightLoggerBase::setFlightAltitudeTouchdown(float altitude) {
  _flightData.altitudeTouchdown = altitude;
}

void flightLoggerBase::setFlightGyroX(float x) {
  _currentRecord.gyroX = x;
}

void flightLoggerBase::setFlightGyroY(float y) {
  _currentRecord.gyroY = y;
}

void flightLoggerBase::setFlightGyroZ(float z) {
  _currentRecord.gyroZ = z;
}

void flightLoggerBase::setFlightHumidity(float humidity) {
  _currentRecord.humidity = humidity;
}

void flightLoggerBase::setFlightPressure(float pressure) {
  _currentRecord.pressure = pressure;
}

void flightLoggerBase::setFlightTemperature(float temperature) {
  _currentRecord.temperature = temperature;
}

void flightLoggerBase::setFlightTime(long diffTime) {
  _currentRecord.diffTime = diffTime;
}

void flightLoggerBase::setFlightTimestampApogee(long diffTime) {
  _flightData.timestampApogee = diffTime;
}

void flightLoggerBase::setFlightTimestampApogeeFirstMeasure(long diffTime) {
  _flightData.timestampApogeeFirstMeasure = diffTime;
}

void flightLoggerBase::setFlightTimestampCurrent(long diffTime) {
  _flightData.timestampCurrent = diffTime;
}

void flightLoggerBase::setFlightTimestampLaunch(long diffTime) {
  _flightData.timestampLaunch = diffTime;
}

void flightLoggerBase::setFlightTimestampPrevious(long diffTime) {
  _flightData.timestampPrevious = diffTime;
}

void flightLoggerBase::setFlightTimestampTouchdown(long diffTime) {
  _flightData.timestampTouchdown = diffTime;
}

void flightLoggerBase::setFlightVelocity(float velocity) {
  _currentRecord.velocity = velocity;
}

bool flightLoggerBase::writeFlight(int flightNbr) {
  return false;
}

bool flightLoggerBase::writeFlightFast() {
  return writeFlight(geFlightNbrLast() + 1);
}
