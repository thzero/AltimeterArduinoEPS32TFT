#include <Arduino.h>

#include "constants.h"
#include "flightLoggerBase.h"
#include "time.h"
#include "utilities.h"

// Add to the _flightDataTraceCurrent to the _flightDataTrace array
bool flightLoggerBase::copyTraceCurrentToArray() {
  flightDataTraceStruct* newFlightData = (flightDataTraceStruct*)realloc(_flightDataTrace, (_flightDataTraceIndex + 1) * sizeof(flightDataTraceStruct));
  if (!newFlightData)
    return false;

  _flightDataTrace = newFlightData;
  _flightDataTrace[_flightDataTraceIndex] = _flightDataTraceCurrent;
  _flightDataTraceIndex++;
  return true;
}

void flightLoggerBase::determineTraceMinAndMax(int flightNbr) {
  _flightDataTraceMinMax.accelXMax = 0;
  _flightDataTraceMinMax.accelXMin = 0;
  _flightDataTraceMinMax.accelYMax = 0;
  _flightDataTraceMinMax.accelYMin = 0;
  _flightDataTraceMinMax.accelZMax = 0;
  _flightDataTraceMinMax.accelZMin = 0;
  _flightDataTraceMinMax.accelZMin = 0;
  _flightDataTraceMinMax.altitudeMax = 0;
  _flightDataTraceMinMax.duration = 0;
  _flightDataTraceMinMax.pressureMax = 0;
  _flightDataTraceMinMax.pressureMin = 0;
  _flightDataTraceMinMax.temperatureMax = 0;
  _flightDataTraceMinMax.temperatureMin = 0;
  _flightDataTraceMinMax.velocityMax = 0;
  _flightDataTraceMinMax.velocityMin = 0;

  if (readFile(flightNbr)) {
    for (int i = 0; i < _flightDataTraceIndex; i++)
    {
      _flightDataTraceMinMax.duration = _flightDataTraceMinMax.duration + _flightDataTrace[i].diffTime;

      if (_flightDataTrace[i].accelX < _flightDataTraceMinMax.accelXMin)
        _flightDataTraceMinMax.accelXMin = _flightDataTrace[i].accelX;
      if (_flightDataTrace[i].accelX > _flightDataTraceMinMax.accelXMax)
        _flightDataTraceMinMax.accelXMax = _flightDataTrace[i].accelX;

      if (_flightDataTrace[i].accelY < _flightDataTraceMinMax.accelYMin)
        _flightDataTraceMinMax.accelXMin = _flightDataTrace[i].accelY;
      if (_flightDataTrace[i].accelY > _flightDataTraceMinMax.accelYMax)
        _flightDataTraceMinMax.accelXMax = _flightDataTrace[i].accelY;

      if (_flightDataTrace[i].accelZ < _flightDataTraceMinMax.accelZMin)
        _flightDataTraceMinMax.accelZMin = _flightDataTrace[i].accelZ;
      if (_flightDataTrace[i].accelZ > _flightDataTraceMinMax.accelZMax)
        _flightDataTraceMinMax.accelZMax = _flightDataTrace[i].accelZ;

      if (_flightDataTrace[i].altitude < _flightDataTraceMinMax.altitudeMin)
        _flightDataTraceMinMax.altitudeMin = _flightDataTrace[i].altitude;
      if (_flightDataTrace[i].altitude > _flightDataTraceMinMax.altitudeMax)
        _flightDataTraceMinMax.altitudeMax = _flightDataTrace[i].altitude;

      if (_flightDataTrace[i].pressure < _flightDataTraceMinMax.pressureMin)
        _flightDataTraceMinMax.pressureMin = _flightDataTrace[i].pressure;
      if (_flightDataTrace[i].pressure > _flightDataTraceMinMax.temperatureMax)
        _flightDataTraceMinMax.pressureMax = _flightDataTrace[i].pressure;

      if (_flightDataTrace[i].temperature < _flightDataTraceMinMax.temperatureMin)
        _flightDataTraceMinMax.temperatureMin = _flightDataTrace[i].temperature;
      if (_flightDataTrace[i].temperature > _flightDataTraceMinMax.temperatureMax)
        _flightDataTraceMinMax.temperatureMax = _flightDataTrace[i].temperature;

      if (_flightDataTrace[i].velocity < _flightDataTraceMinMax.velocityMin)
        _flightDataTraceMinMax.temperatureMin = _flightDataTrace[i].velocity;
      if (_flightDataTrace[i].velocity > _flightDataTraceMinMax.velocityMax)
        _flightDataTraceMinMax.velocityMax = _flightDataTrace[i].velocity;
    }
  }
}

bool flightLoggerBase::eraseLast() {
}

bool flightLoggerBase::eraseFlights() {
  return false;
}

bool flightLoggerBase::exists(int flightNbr) {
  return false;
}

float flightLoggerBase::getAccelXMax() {
  return _flightDataTraceMinMax.accelXMax;
}

float flightLoggerBase::getAccelXMin() {
  return _flightDataTraceMinMax.accelXMin;
}

float flightLoggerBase::getAccelYMax() {
  return _flightDataTraceMinMax.accelYMax;
}

float flightLoggerBase::getAccelYMin() {
  return _flightDataTraceMinMax.accelYMin;
}

float flightLoggerBase::getAccelZMax() {
  return _flightDataTraceMinMax.accelZMax;
}

float flightLoggerBase::getAccelZMin() {
  return _flightDataTraceMinMax.accelZMin;
}

float flightLoggerBase::getAltitudeMax() {
  return _flightDataTraceMinMax.altitudeMax;
}

float flightLoggerBase::getltitudeMin() {
  return _flightDataTraceMinMax.altitudeMin;
}

flightDataStruct flightLoggerBase::getData() {
  return _flightData;
}

flightDataTraceStruct* flightLoggerBase::getDataTrace() {
  return _flightDataTrace;
}

long flightLoggerBase::getDataTraceSize() {
  return _flightDataTraceIndex;
}

long flightLoggerBase::getDuration() {
  return _flightDataTraceMinMax.duration;
}

float flightLoggerBase::getHumidityMax() {
  return _flightDataTraceMinMax.humidityMax;
}

float flightLoggerBase::getHumidityMin() {
  return _flightDataTraceMinMax.humidityMin;
}

flightDataNumberStruct flightLoggerBase::geFlightNbrs() {
  flightDataNumberStruct results;
  results.numbers = _flightNumbers;
  results.size = _flightNumbersSize;
  return results;
}

long flightLoggerBase::geFlightNbrsLast() {
  return _flightNumbersLast;
}

float flightLoggerBase::getPressureMax() {
  return _flightDataTraceMinMax.pressureMax;
}

float flightLoggerBase::getFlightPressureMin() {
  return _flightDataTraceMinMax.pressureMin;
}

float flightLoggerBase::getTemperatureMax() {
  return _flightDataTraceMinMax.temperatureMax;
}

float flightLoggerBase::getTemperatureMin() {
  return _flightDataTraceMinMax.temperatureMin;
}

float flightLoggerBase::getVelocityMax() {
  return _flightDataTraceMinMax.velocityMax;
}

float flightLoggerBase::getVelocityMin() {
  return _flightDataTraceMinMax.velocityMin;
}

void flightLoggerBase::init(unsigned long timestamp) {
  reset();
  _flightData.epochS = getEpoch();
  _flightData.timestampLaunch = timestamp;
}

bool flightLoggerBase::initFileSystem() {
  return false;
}

bool flightLoggerBase::listAsJson(JsonArray flightLogs) {
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
    Serial.println(F("\tflightLoggerLFS.listAsJson..."));
#endif
  return false;
}

void flightLoggerBase::outputSerial() {
  unsigned long currentTime = 0;

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.println(F("outputSerial...base..."));
#endif

  char format[4] = "%i,";
  char formatF[6] = "%.2f,";

  char flightDt[120] = "";
  char temp[20] = "";
  for (long i = 0; i < _flightDataTraceIndex; i++) {
    // memset(flightDt, 0, sizeof flightDt);
    // memset(temp, 0, sizeof temp);
    currentTime = currentTime + _flightDataTrace[i].diffTime;
    strcpy(flightDt, "data,");
    sprintf(temp, format, _flightDataTraceIndex - 1);
    strcat(flightDt, temp);
    sprintf(temp, format, currentTime);
    strcat(flightDt, temp);
    sprintf(temp, format, _flightDataTrace[i].altitude);
    strcat(flightDt, temp);
    sprintf(temp, format, _flightDataTrace[i].temperature);
    strcat(flightDt, temp);
    sprintf(temp, format, _flightDataTrace[i].pressure);
    strcat(flightDt, temp);
    //sprintf(temp, format, _flightDataTrace[i].humidity); //humidity
    ///strcat(flightDt, temp);
    sprintf(temp, format, _flightDataTrace[i].accelX * 1000);
    strcat(flightDt, temp);
    sprintf(temp, format, _flightDataTrace[i].accelY * 1000);
    strcat(flightDt, temp);
    sprintf(temp, format, _flightDataTrace[i].accelZ * 1000);
    strcat(flightDt, temp);

    unsigned int chk = msgChk(flightDt, sizeof(flightDt));
    sprintf(temp, "%i", chk);
    strcat(flightDt, temp);
    strcat(flightDt, ";\n");

    Serial.print("$");
    Serial.print(flightDt);
  }

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.println(F("...outputSerial...base...finished"));
#endif
}

void flightLoggerBase::outputSerial(int flightNbr) {
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("outputSerial...base...#"));
  Serial.println(flightNbr);
#endif
  
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("outputSerial...base...#...read file"));
  Serial.println(flightNbr);
#endif
  if (!readFile(flightNbr)) {
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
      Serial.print(F("...outputSerial...base...#"));
      Serial.print(flightNbr);
      Serial.println(F("...failed to read file"));
#else
      Serial.print(F("Failed to read flight #"));
      Serial.println(flightNbr);
#endif
    return;
  }

  outputSerial();
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("...outputSerial...base...#"));
  Serial.print(flightNbr);
  Serial.println(F("...finished"));
#endif
}

void flightLoggerBase::outputSerialExpanded() {
  unsigned long currentTime = 0;

  char format[4] = "%i,";
  char formatF[6] = "%.2f,";

  Serial.print(F("$data,"));
  Serial.print(_flightData.epochS);
  Serial.print(F(","));
  Serial.print(_flightData.altitudeApogee);
  Serial.print(F(","));
  Serial.print(_flightData.altitudeInitial);
  Serial.print(F(","));
  Serial.print(_flightData.altitudeLaunch);
  Serial.print(F(","));
  Serial.print(_flightData.altitudeTouchdown);
  Serial.print(F(","));
  Serial.print(_flightData.timestampLaunch);
  Serial.print(F(","));
  Serial.print(_flightData.timestampApogee);
  Serial.print(F(","));
  Serial.print(_flightData.timestampTouchdown);
  Serial.println(F(";"));

  char flightDt[120] = "";
  char temp[20] = "";
  for (int i = 0; i < _flightDataTraceIndex; i++) {
    // memset(flightDt, 0, sizeof flightDt);
    // memset(temp, 0, sizeof temp);
    currentTime = currentTime + _flightDataTrace[i].diffTime;
    sprintf(temp, format, currentTime);
    strcpy(flightDt, temp);
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

    Serial.print(F("$trace,"));
    Serial.print(flightDt);
  }
}

void flightLoggerBase::outputSerialExpanded(int flightNbr) {
  if (!readFile(flightNbr))
    return;

  outputSerialExpanded();
}

bool flightLoggerBase::outputSerialList() {
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.println(F("Output flight log list to serial..."));
#endif
  DynamicJsonDocument doc(4096);
  deserializeJson(doc, "[]");
  JsonArray flightLogs = doc.as<JsonArray>();
  listAsJson(flightLogs);

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  serializeJson(flightLogs, Serial);
  Serial.println(F(""));
#endif
  
  Serial.println(F("$start;"));
  for (JsonObject obj : flightLogs) {
    String number = obj["number"];
    Serial.print(F("$data,"));
    Serial.print(number);
    Serial.print(F(","));
    unsigned long epochS = obj["epochS"];
    // Serial.print(convertTime(epochS));
    Serial.print(epochS);
    Serial.println(F(";"));
  }
  Serial.println(F("$end;"));

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.println(F("...output of flight logs list...finished"));
#endif
  return true;
}

void flightLoggerBase::reset() {
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
  _flightDataTraceIndex = 0;
}

bool flightLoggerBase::readFile(int flightNbr) {
}

JsonObject flightLoggerBase::readFileAsJson(int flightNbr) {
}

int flightLoggerBase::reindexFlights() {
}

void flightLoggerBase::setTraceCurrentAccelX(float x) {
  _flightDataTraceCurrent.accelX = x;
}

void flightLoggerBase::setTraceCurrentAccelY(float y) {
  _flightDataTraceCurrent.accelY = y;
}

void flightLoggerBase::setTraceCurrentAccelZ(float z) {
  _flightDataTraceCurrent.accelZ = z;
}

void flightLoggerBase::setTraceCurrentAltitude(float altitude) {
  _flightDataTraceCurrent.altitude = altitude;
}

void flightLoggerBase::setAltitudeApogee(float altitude) {
  _flightData.altitudeApogee = altitude;
}

void flightLoggerBase::setAltitudeApogeeFirstMeasure(float altitude) {
  _flightData.altitudeApogeeFirstMeasure = altitude;
}

void flightLoggerBase::setAltitudeCurrent(float altitude) {
  _flightData.altitudeCurrent = altitude;
}

void flightLoggerBase::setAltitudeLast(float altitude) {
  _flightData.altitudeLast = altitude;
}

void flightLoggerBase::setAltitudeLaunch(float altitude) {
  _flightData.altitudeLaunch = altitude;
}

void flightLoggerBase::setAltitudeTouchdown(float altitude) {
  _flightData.altitudeTouchdown = altitude;
}

void flightLoggerBase::setTraceCurrentGyroX(float x) {
  _flightDataTraceCurrent.gyroX = x;
}

void flightLoggerBase::setTraceCurrentGyroY(float y) {
  _flightDataTraceCurrent.gyroY = y;
}

void flightLoggerBase::setTraceCurrentGyroZ(float z) {
  _flightDataTraceCurrent.gyroZ = z;
}

void flightLoggerBase::setTraceCurrentHumidity(float humidity) {
  _flightDataTraceCurrent.humidity = humidity;
}

void flightLoggerBase::setTraceCurrentPressure(float pressure) {
  _flightDataTraceCurrent.pressure = pressure;
}

void flightLoggerBase::setTraceCurrentTemperature(float temperature) {
  _flightDataTraceCurrent.temperature = temperature;
}

void flightLoggerBase::setTraceCurrentTime(long diffTime) {
  _flightDataTraceCurrent.diffTime = diffTime;
}

void flightLoggerBase::setTimestampApogee(long diffTime) {
  _flightData.timestampApogee = diffTime;
}

void flightLoggerBase::setTimestampApogeeFirstMeasure(long diffTime) {
  _flightData.timestampApogeeFirstMeasure = diffTime;
}

void flightLoggerBase::setTimestampCurrent(long diffTime) {
  _flightData.timestampCurrent = diffTime;
}

void flightLoggerBase::setTimestampLaunch(long diffTime) {
  _flightData.timestampLaunch = diffTime;
}

void flightLoggerBase::setTimestampPrevious(long diffTime) {
  _flightData.timestampPrevious = diffTime;
}

void flightLoggerBase::setTimestampTouchdown(long diffTime) {
  _flightData.timestampTouchdown = diffTime;
}

void flightLoggerBase::setTraceCurrentVelocity(float velocity) {
  _flightDataTraceCurrent.velocity = velocity;
}

bool flightLoggerBase::writeFile(int flightNbr) {
  return false;
}

bool flightLoggerBase::writeFlightCurrent() {
  return writeFile(geFlightNbrsLast() + 1);
}
