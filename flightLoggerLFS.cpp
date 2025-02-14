#include <ArduinoJson.h>
#include <LittleFS.h>

#include "flightLoggerLFS.h"
#include "utilities.h"

flightLoggerLFS::flightLoggerLFS() {
  _flightData = nullptr;
  _dataPos = 0;
}

bool flightLoggerLFS::clearFlightList() {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
  Serial.println(F("Clearing all flights..."));
#endif

  // Open the root directory
  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
    Serial.println(F("Failed to open the root directory"));
#endif
    return false;
  }

  // Iterate over all files in the root directory
  File file = root.openNextFile();
  while (file) {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
    Serial.print(F("Deleting file: "));
    Serial.println(file.name());
#endif

    // Delete the current file
    char fileName[15];
    sprintf(fileName, "/%s", file.name());
    //make sure that you close the file so that it can be deleted
    file.close();

    //if (LittleFS.remove(file.name())) {
    if (LittleFS.remove(fileName)) {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
      Serial.println(F("Flight deleted successfully"));
#endif
    } 
    else {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
      Serial.println(F("Failed to delete flight"));
#endif
    }

    // Move to the next file
    file = root.openNextFile();
  }

#if defined(DEBUG) || defined(DEBUG_LOGGER)
  Serial.println(F("All files cleared."));
#endif
  return true;
}

void flightLoggerLFS::determineFlightMinAndMax(int flightNbr) {
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
      _flightMinAndMax.duration = _flightMinAndMax.duration + _flightData[i].diffTime;

      if (_flightData[i].accelX < _flightMinAndMax.accelXMin)
        _flightMinAndMax.accelXMin = _flightData[i].accelX;
      if (_flightData[i].accelX > _flightMinAndMax.accelXMax)
        _flightMinAndMax.accelXMax = _flightData[i].accelX;

      if (_flightData[i].accelY < _flightMinAndMax.accelYMin)
        _flightMinAndMax.accelXMin = _flightData[i].accelY;
      if (_flightData[i].accelY > _flightMinAndMax.accelYMax)
        _flightMinAndMax.accelXMax = _flightData[i].accelY;

      if (_flightData[i].accelZ < _flightMinAndMax.accelZMin)
        _flightMinAndMax.accelZMin = _flightData[i].accelZ;
      if (_flightData[i].accelZ > _flightMinAndMax.accelZMax)
        _flightMinAndMax.accelZMax = _flightData[i].accelZ;

      if (_flightData[i].altitude < _flightMinAndMax.altitudeMin)
        _flightMinAndMax.altitudeMin = _flightData[i].altitude;
      if (_flightData[i].altitude > _flightMinAndMax.altitudeMax)
        _flightMinAndMax.altitudeMax = _flightData[i].altitude;

      if (_flightData[i].pressure < _flightMinAndMax.pressureMin)
        _flightMinAndMax.pressureMin = _flightData[i].pressure;
      if (_flightData[i].pressure > _flightMinAndMax.temperatureMax)
        _flightMinAndMax.pressureMax = _flightData[i].pressure;

      if (_flightData[i].temperature < _flightMinAndMax.temperatureMin)
        _flightMinAndMax.temperatureMin = _flightData[i].temperature;
      if (_flightData[i].temperature > _flightMinAndMax.temperatureMax)
        _flightMinAndMax.temperatureMax = _flightData[i].temperature;

      if (_flightData[i].velocity < _flightMinAndMax.velocityMin)
        _flightMinAndMax.temperatureMin = _flightData[i].velocity;
      if (_flightData[i].velocity > _flightMinAndMax.velocityMax)
        _flightMinAndMax.velocityMax = _flightData[i].velocity;
    }
  }
}

long flightLoggerLFS::getFlightDuration() {
  return _flightMinAndMax.duration;
}

float flightLoggerLFS::getFlightAccelXMax() {
  return _flightMinAndMax.accelXMax;
}

float flightLoggerLFS::getFlightAccelXMin() {
  return _flightMinAndMax.accelXMin;
}

float flightLoggerLFS::getFlightAccelYMax() {
  return _flightMinAndMax.accelYMax;
}

float flightLoggerLFS::getFlightAccelYMin() {
  return _flightMinAndMax.accelYMin;
}

float flightLoggerLFS::getFlightAccelZMax() {
  return _flightMinAndMax.accelZMax;
}

float flightLoggerLFS::getFlightAccelZMin() {
  return _flightMinAndMax.accelZMin;
}

float flightLoggerLFS::getFlightAltitudeMax() {
  return _flightMinAndMax.altitudeMax;
}

float flightLoggerLFS::getFlightAltitudeMin() {
  return _flightMinAndMax.altitudeMin;
}

flightDataStruct* flightLoggerLFS::getFlightData() {
  return _flightData;
}

float flightLoggerLFS::getFlightHumidityMax() {
  return _flightMinAndMax.humidityMax;
}

float flightLoggerLFS::getFlightHumidityMin() {
  return _flightMinAndMax.humidityMin;
}

float flightLoggerLFS::getFlightPressureMax() {
  return _flightMinAndMax.pressureMax;
}

float flightLoggerLFS::getFlightPressureMin() {
  return _flightMinAndMax.pressureMin;
}

float flightLoggerLFS::getFlightTemperatureMax() {
  return _flightMinAndMax.temperatureMax;
}

float flightLoggerLFS::getFlightTemperatureMin() {
  return _flightMinAndMax.temperatureMin;
}

float flightLoggerLFS::getFlightVelocityMax() {
  return _flightMinAndMax.velocityMax;
}

float flightLoggerLFS::getFlightVelocityMin() {
  return _flightMinAndMax.velocityMin;
}

long flightLoggerLFS::geFlightNbrLast() {
  long maxFlightNumber = 0; // Default to 0 if no valid flight file is found

  // Open the root directory
  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
    Serial.println(F("Failed to open the root directory"));
#endif
    return maxFlightNumber;
  }

  // Iterate over all files in the root directory
  File file = root.openNextFile();
  while (file) {
    String fileName = file.name();
#if defined(DEBUG) || defined(DEBUG_LOGGER)
    Serial.print(F("Found file: "));
    Serial.println(fileName);
#endif

    // Check if the filename matches the pattern "flight<number>.json"
    if (fileName.startsWith("flight") && fileName.endsWith(".json")) {
      // Extract the flight number
      String numberPart = fileName.substring(6, fileName.length() - 5); // Extract between "flight" and ".json"
      int flightNumber = numberPart.toInt(); // Convert to integer

      // Update the maxFlightNumber if this one is larger
      if (flightNumber > maxFlightNumber) {
        maxFlightNumber = flightNumber;
      }
    }

    // Move to the next file
    file = root.openNextFile();
  }

  return maxFlightNumber;
}

long flightLoggerLFS::getFlightSize() {
  return _dataPos;
}

bool flightLoggerLFS::initFileSystem() {
  if (!LittleFS.begin(false)) {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
    Serial.println(F("LittleFS mount failed");
    Serial.println(F("Did not find filesystem; starting format"));
#endif

    // format if begin fails
    if (!LittleFS.begin(true)) {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
      Serial.println(F("LittleFS mount failed"));
      Serial.println(F("Formatting not possible"));
#endif
      return false;
    }

    Serial.println(F("Formatting"));
  }

  return true;
}

bool flightLoggerLFS::initFlight() {
  if (_flightData != nullptr)
    free(_flightData);

  _flightData = nullptr;
  _dataPos = 0;
  return true;
}

void flightLoggerLFS::printFlightData(int flightNbr) {
  unsigned long currentTime = 0;

  if (!readFlight(flightNbr))
    return;

  char format[4] = "%i,";
  char formatF[6] = "%.2f,";

  for (int i = 0; i < _dataPos ; i++)
  {
    char flightDt[120] = "";
    char temp[20] = "";
    currentTime = currentTime + _flightData[i].diffTime;
    strcat(flightDt, "data,");
    sprintf(temp, format, flightNbr-1);
    strcat(flightDt, temp);
    sprintf(temp, format, currentTime);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightData[i].accelX * 1000);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightData[i].accelY * 1000);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightData[i].accelZ * 1000);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightData[i].altitude);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightData[i].gyroX * 1000);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightData[i].gyroY * 1000);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightData[i].gyroZ * 1000);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightData[i].humidity);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightData[i].pressure);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightData[i].temperature);
    strcat(flightDt, temp);
    sprintf(temp, formatF, _flightData[i].velocity);
    strcat(flightDt, temp);

    unsigned int chk = msgChk(flightDt, sizeof(flightDt));
    sprintf(temp, "%i", chk);
    strcat(flightDt, temp);
    strcat(flightDt, ";\n");

    Serial.print(F("$"));
    Serial.print(flightDt);
  }
}

bool flightLoggerLFS::writeFlightFast() {
  return writeFlight(geFlightNbrLast() + 1);
}

void flightLoggerLFS::setFlightAccelX(float x) {
  _currentRecord.accelX = x;
}

void flightLoggerLFS::setFlightAccelY(float y) {
  _currentRecord.accelY = y;
}

void flightLoggerLFS::setFlightAccelZ(float z) {
  _currentRecord.accelZ = z;
}

void flightLoggerLFS::setFlightAltitude(float altitude) {
  _currentRecord.altitude = altitude;
}

bool flightLoggerLFS::setFlightData() {
  flightDataStruct* newFlightData = (flightDataStruct*)realloc(_flightData, (_dataPos + 1) * sizeof(flightDataStruct));
  if (!newFlightData)
    return false;

  _flightData = newFlightData;
  _flightData[_dataPos] = _currentRecord;
  _dataPos++;
  return true;
}

void flightLoggerLFS::setFlightGyroX(float x) {
  _currentRecord.gyroX = x;
}

void flightLoggerLFS::setFlightGyroY(float y) {
  _currentRecord.gyroY = y;
}

void flightLoggerLFS::setFlightGyroZ(float z) {
  _currentRecord.gyroZ = z;
}

void flightLoggerLFS::setFlightHumidity(float humidity) {
  _currentRecord.humidity = humidity;
}

void flightLoggerLFS::setFlightPressure(float pressure) {
  _currentRecord.pressure = pressure;
}

void flightLoggerLFS::setFlightTemperature(float temperature) {
  _currentRecord.temperature = temperature;
}

void flightLoggerLFS::setFlightTime(long diffTime) {
  _currentRecord.diffTime = diffTime;
}

void flightLoggerLFS::setFlightVelocity(float velocity) {
  _currentRecord.velocity = velocity;
}

bool flightLoggerLFS::readFlight(int flightNbr) {
  char flightName [15];
  sprintf(flightName, "/flight%i.json", flightNbr);
#if defined(DEBUG) || defined(DEBUG_LOGGER)
  Serial.println(flightName);
#endif

  File file = LittleFS.open(flightName, "r");
  if (!file) 
    return false;

  DynamicJsonDocument doc(4096);
  deserializeJson(doc, file);
  file.close();

  JsonObject flight = doc[flightName];
  if (flight.isNull())
    return false;

  _dataPos = flight.size();
  if (_flightData != nullptr)
    free(_flightData);

  _flightData = (flightDataStruct*)malloc(sizeof(flightDataStruct) * _dataPos);
  if (!_flightData)
    return false;

  long index = 0;
  for (JsonPair pair : flight) {
    JsonObject record = pair.value().as<JsonObject>();
    _flightData[index].diffTime = record["diffTime"];
    _flightData[index].accelX = record["accelX"];
    _flightData[index].accelY = record["accelY"];
    _flightData[index].accelZ = record["accelZ"];
    _flightData[index].altitude = record["altitude"];
    _flightData[index].gyroX = record["gyroX"];
    _flightData[index].gyroY = record["gyroY"];
    _flightData[index].gyroZ = record["gyroZ"];
    _flightData[index].humidity = record["humidity"];
    _flightData[index].pressure = record["pressure"];
    _flightData[index].temperature = record["temperature"];
    _flightData[index].velocity = record["velocity"];
    index++;
  }

  return true;
}

bool flightLoggerLFS::writeFlight(int flightNbr) {
  char flightName [15];
  sprintf(flightName, "/flight%i.json", flightNbr);

  DynamicJsonDocument doc(4096);
  JsonObject flight = doc.createNestedObject(flightName);

  for (long i = 0; i < _dataPos; i++) {
    JsonObject record = flight.createNestedObject(String(i));
    record["diffTime"] = _flightData[i].diffTime;
    record["accelX"] = _flightData[i].accelX;
    record["accelY"] = _flightData[i].accelY;
    record["accelZ"] = _flightData[i].accelZ;
    record["altitude"] = _flightData[i].altitude;
    record["gyroX"] = _flightData[i].gyroX;
    record["gyroY"] = _flightData[i].gyroY;
    record["gyroZ"] = _flightData[i].gyroZ;
    record["humidity"] = _flightData[i].humidity;
    record["pressure"] = _flightData[i].pressure;
    record["temperature"] = _flightData[i].temperature;
    record["velocity"] = _flightData[i].velocity;
  }

  // Serial.println(String(doc)));
  File file = LittleFS.open(flightName, "w");
  serializeJson(doc, file);
  file.close();

  return true;
}

// unsigned int flightLoggerLFS::msgChk( char * buffer, long length) {
//   long index;
//   unsigned int checksum;

//   for ( index = 0L, checksum = 0; index < length; checksum += (unsigned int) buffer[index++]);
//   return (unsigned int) (checksum % 256);
// }
