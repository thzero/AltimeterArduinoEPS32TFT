#include <ArduinoJson.h>
#include <LittleFS.h>

#include "flightLoggerLFS.h"
#include "utilities.h"

flightLoggerLFS::flightLoggerLFS() {
  _flightDataTrace = nullptr;
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

bool flightLoggerLFS::initFileSystem() {
//   if (!LittleFS.begin(false)) {
// #if defined(DEBUG) || defined(DEBUG_LOGGER)
//     Serial.println(F("LittleFS mount failed");
//     Serial.println(F("Did not find filesystem; starting format"));
// #endif

//     // format if begin fails
//     if (!LittleFS.begin(true)) {
// #if defined(DEBUG) || defined(DEBUG_LOGGER)
//       Serial.println(F("LittleFS mount failed"));
//       Serial.println(F("Formatting not possible"));
// #endif
//       return false;
//     }

//     Serial.println(F("Formatting"));
//   }

  return true;
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
  if (_flightDataTrace != nullptr)
    free(_flightDataTrace);

  _flightDataTrace = (flightDataTraceStruct*)malloc(sizeof(flightDataTraceStruct) * _dataPos);
  if (!_flightDataTrace)
    return false;

  long index = 0;
  for (JsonPair pair : flight) {
    JsonObject record = pair.value().as<JsonObject>();
    _flightDataTrace[index].diffTime = record["diffTime"];
    _flightDataTrace[index].accelX = record["accelX"];
    _flightDataTrace[index].accelY = record["accelY"];
    _flightDataTrace[index].accelZ = record["accelZ"];
    _flightDataTrace[index].altitude = record["altitude"];
    _flightDataTrace[index].gyroX = record["gyroX"];
    _flightDataTrace[index].gyroY = record["gyroY"];
    _flightDataTrace[index].gyroZ = record["gyroZ"];
    _flightDataTrace[index].humidity = record["humidity"];
    _flightDataTrace[index].pressure = record["pressure"];
    _flightDataTrace[index].temperature = record["temperature"];
    _flightDataTrace[index].velocity = record["velocity"];
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
    record["diffTime"] = _flightDataTrace[i].diffTime;
    record["accelX"] = _flightDataTrace[i].accelX;
    record["accelY"] = _flightDataTrace[i].accelY;
    record["accelZ"] = _flightDataTrace[i].accelZ;
    record["altitude"] = _flightDataTrace[i].altitude;
    record["gyroX"] = _flightDataTrace[i].gyroX;
    record["gyroY"] = _flightDataTrace[i].gyroY;
    record["gyroZ"] = _flightDataTrace[i].gyroZ;
    record["humidity"] = _flightDataTrace[i].humidity;
    record["pressure"] = _flightDataTrace[i].pressure;
    record["temperature"] = _flightDataTrace[i].temperature;
    record["velocity"] = _flightDataTrace[i].velocity;
  }

  // Serial.println(String(doc)));
  File file = LittleFS.open(flightName, "w");
  serializeJson(doc, file);
  file.close();

  return true;
}