#include <ArduinoJson.h>
#include <LittleFS.h>

#include "constants.h"
#include "debug.h"
#include "flightLoggerLFS.h"
#include "utilities.h"

flightLoggerLFS::flightLoggerLFS() {
  _flightDataTrace = nullptr;
  _flightDataTraceIndex = 0;
}

bool flightLoggerLFS::clearFlights() {
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

bool flightLoggerLFS::exists(int flightNbr) {
  char flightName [15];
  sprintf(flightName, "/flight%i.json", flightNbr);
#if defined(DEBUG) || defined(DEBUG_LOGGER)
  Serial.print(F("existsFlight..."));
  Serial.println(flightName);
#endif

  File file = LittleFS.open(flightName, "r");
  if (!file) 
    return false;

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
  Serial.println(F("\tInitialize flightLogger file system..."));

  Serial.println(F("\t...flightLogger file system initialized."));

  return true;
}

bool flightLoggerLFS::readFile(int flightNbr) {
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

  JsonObject flightLog = doc.as<JsonObject>();
  if (flightLog.isNull())
    return false;

  _flightDataTraceIndex = flightLog.size();
  if (_flightDataTrace != nullptr)
    free(_flightDataTrace);

  _flightDataTrace = (flightDataTraceStruct*)malloc(sizeof(flightDataTraceStruct) * _flightDataTraceIndex);
  if (!_flightDataTrace)
    return false;
    
  _flightData.epochS = flightLog["epochS"];
  _flightData.altitudeApogee = flightLog["altitudeApogee"];
  _flightData.altitudeInitial = flightLog["altitudeInitial"];
  _flightData.altitudeLaunch = flightLog["altitudeLaunch"];
  _flightData.altitudeTouchdown = flightLog["altitudeTouchdown"];
  _flightData.timestampLaunch = flightLog["timestampLaunch"];
  _flightData.timestampApogee = flightLog["timestampApogee"];
  _flightData.timestampTouchdown = flightLog["timestampTouchdown"];

  JsonArray traces = (flightLog["traces"]).as<JsonArray>();
  long index = 0;
  for (JsonVariant record : traces) {
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

flightDataReadResultsStruct flightLoggerLFS::readFileAsJson(int flightNbr) {
  char flightName [15];
  sprintf(flightName, "/flight%i.json", flightNbr);
#if defined(DEBUG) || defined(DEBUG_LOGGER)
  Serial.println(flightName);
#endif

  flightDataReadResultsStruct results;

  File file = LittleFS.open(flightName, "r");
  if (!file) {
    results.success = false;
    return results;
  }

  DynamicJsonDocument doc(4096);
  deserializeJson(doc, file);
  file.close();
  results.results = doc.as<JsonObject>();

  results.success = true;
  return results;
}

void flightLoggerLFS::readFlightsAsJson(JsonArray flightLogs) {
  #ifdef DEBUG
    Serial.println(F("\tflightLoggerLFS.readFlightsAsJson..."));
  #endif
  
  // Open the root directory
  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
    Serial.println(F("Failed to open the root directory"));
#endif
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
    if (!(fileName.startsWith("flight") && fileName.endsWith(".json"))) {
      // Move to the next file
      file = root.openNextFile();
      continue;
    }

    DynamicJsonDocument doc(4096);
    deserializeJson(doc, file);

    JsonObject flightLog = flightLogs.createNestedObject();
    flightLog["number"] = doc["number"];
    flightLog["epochS"] = doc["epochS"];

    // Move to the next file
    file = root.openNextFile();
  }

  #ifdef DEBUG
  Serial.println(F("\treadFlightsAsJson"));
  serializeJson(flightLogs, Serial);
  Serial.println("");
  #endif

  #ifdef DEBUG
    Serial.println(F("\tflightLoggerLFS.readFlightsAsJson...finisshed"));
  #endif
}

bool flightLoggerLFS::writeFile(int flightNbr) {
  char flightName [15];
  sprintf(flightName, "/flight%i.json", flightNbr);

  DynamicJsonDocument doc(4096);
  JsonObject flightLog = doc.as<JsonObject>();
  flightLog["number"] = flightNbr;
  flightLog["epochS"] = _flightData.epochS;
  flightLog["altitudeApogee"] = _flightData.altitudeApogee;
  flightLog["altitudeInitial"] = _flightData.altitudeInitial;
  flightLog["altitudeLaunch"] = _flightData.altitudeLaunch;
  flightLog["altitudeTouchdown"] = _flightData.altitudeTouchdown;
  flightLog["timestampLaunch"] = _flightData.timestampLaunch;
  flightLog["timestampApogee"] = _flightData.timestampApogee;
  flightLog["timestampTouchdown"] = _flightData.timestampTouchdown;

  JsonArray traces = flightLog.createNestedArray("traces");

  for (long i = 0; i < _flightDataTraceIndex; i++) {
    JsonObject trace = traces.createNestedObject();
    trace["diffTime"] = _flightDataTrace[i].diffTime;
    trace["accelX"] = _flightDataTrace[i].accelX;
    trace["accelY"] = _flightDataTrace[i].accelY;
    trace["accelZ"] = _flightDataTrace[i].accelZ;
    trace["altitude"] = _flightDataTrace[i].altitude;
    trace["gyroX"] = _flightDataTrace[i].gyroX;
    trace["gyroY"] = _flightDataTrace[i].gyroY;
    trace["gyroZ"] = _flightDataTrace[i].gyroZ;
    trace["humidity"] = _flightDataTrace[i].humidity;
    trace["pressure"] = _flightDataTrace[i].pressure;
    trace["temperature"] = _flightDataTrace[i].temperature;
    trace["velocity"] = _flightDataTrace[i].velocity;
  }

  // Serial.println(String(doc)));
  File file = LittleFS.open(flightName, "w");
  serializeJson(doc, file);
  file.close();

  return true;
}