#include <ArduinoJson.h>
#include <LittleFS.h>

#include "constants.h"
#include "debug.h"
#include "flightLoggerLFS.h"
#include "time.h"
#include "utilities.h"

flightLoggerLFS::flightLoggerLFS() {
  _flightDataTrace = nullptr;
  _flightDataTraceIndex = 0;
}

bool flightLoggerLFS::eraseFlights() {
  Serial.println(F("Clearing all flights..."));

  // Open the root directory
  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
    Serial.println(F("...failed to open the root directory"));
#endif
    return false;
  }

  // Iterate over all files in the root directory
  File file = root.openNextFile();
  while (file) {
    String fileName = file.name();
#if defined(DEBUG) || defined(DEBUG_LOGGER)
    Serial.print(F("\tDeleting file: "));
    Serial.println(fileName);
#endif

    // Check if the filename matches the pattern "flight<number>.json" or "flightIndex.json"
    if (!(fileName.startsWith("flight") && fileName.endsWith(".json"))) {
      // Move to the next file
      file = root.openNextFile();
      continue;
    }

    //make sure that you close the file so that it can be deleted
    file.close();

    //if (LittleFS.remove(file.name())) {
    if (LittleFS.remove(fileName)) {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
      Serial.print(F("\tFlight '"));
      Serial.print(fileName);
      Serial.println(F("' deleted successfully"));
#endif
    } 
    else {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
      Serial.print(F("\tFlight '"));
      Serial.print(fileName);
      Serial.println(F("' delete failed"));
#endif
    }

    // Move to the next file
    file = root.openNextFile();
  }

  Serial.println(F("...all files cleared."));
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
    Serial.print(F("\nFound file: "));
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

bool flightLoggerLFS::listAsJson(JsonArray flightLogs) {
  Serial.println(F("Retrieving flight log index..."));
  
  File file = LittleFS.open("/flightIndex.json");
  if (!file || file.isDirectory()) {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
    Serial.println(F("\t...failed to open the flight index"));
#endif
    return false;
  }

  DynamicJsonDocument doc(4096);
  deserializeJson(doc, file);

#ifdef DEBUG
  Serial.println(F("\tlistAsJson"));
  serializeJson(doc, Serial);
  Serial.println(F(""));
#endif

  for (JsonObject obj : doc.as<JsonArray>())
    flightLogs.add(obj);

#ifdef DEBUG
  Serial.println(F("\tlistAsJson2"));
  serializeJson(flightLogs, Serial);
  Serial.println(F(""));
#endif

  Serial.println(F("...flight log index retrieval...finished"));
  return true;
}

bool flightLoggerLFS::outputSerialList() {
  Serial.println(F("Output flight log list to serial..."));
  
  // Open the root directory
  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
    Serial.println(F("...failed to open the root directory"));
#endif
    return false;
  }

  Serial.println(F("Number\tEpcoh (seconds)"));

  // Iterate over all files in the root directory
  File file = root.openNextFile();
  while (file) {
    String fileName = file.name();
#if defined(DEBUG) || defined(DEBUG_LOGGER)
    Serial.print(F("Found file: "));
    Serial.println(fileName);
#endif

    // Check if the filename matches the pattern "flight<number>.json"
    if (!(fileName.startsWith("flight") && fileName.endsWith(".json")) || fileName.startsWith("flightIndex")) {
      // Move to the next file
      file = root.openNextFile();
      continue;
    }

    DynamicJsonDocument doc(4096);
    deserializeJson(doc, file);

    String number = doc["number"];
    Serial.print(number);
    Serial.print(F("\t\t"));
    unsigned long epochS = doc["epochS"];
    // Serial.print(convertTime(epochS));
    Serial.print(epochS);
    Serial.println(F(""));

    // Move to the next file
    file = root.openNextFile();
  }

  Serial.println(F("...output of flight logs list...finished"));
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

bool flightLoggerLFS::reindexFlights() {
  Serial.println(F("\t\t\t\tReindex flight logs index..."));
  
  // Open the root directory
  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
    Serial.println(F("\t\t\t\t...failed to open the root directory"));
#endif
    return false;
  }
  
  DynamicJsonDocument doc(4096);
  char json[] = "[]";
  deserializeJson(doc, json);
  JsonArray flightLogs = doc.as<JsonArray>();

  DynamicJsonDocument doc2(4096);
  // Iterate over all files in the root directory
  File file = root.openNextFile();
  while (file) {
    String fileName = file.name();
// #if defined(DEBUG) || defined(DEBUG_LOGGER)
//     Serial.print(F("Found file: "));
//     Serial.println(fileName);
// #endif
    // Check if the filename matches the pattern "flight<number>.json"
    if (!(fileName.startsWith("flight") && fileName.endsWith(".json")) || fileName.startsWith("flightIndex")) {
      // Move to the next file
      file = root.openNextFile();
      continue;
    }

    Serial.print(F("\t\t\t\tFlight file: "));
    Serial.print(fileName);
    Serial.println(F(""));

    deserializeJson(doc2, file);

    JsonObject flightLog = flightLogs.createNestedObject();
    flightLog["number"] = doc2["number"];
    flightLog["epochS"] = doc2["epochS"];

    // Move to the next file
    file = root.openNextFile();
  }

#ifdef DEBUG
  Serial.println(F("\t\t\t\t"));
  serializeJson(doc, Serial);
  Serial.println(F(""));
#endif

  File fileW = LittleFS.open("/flightIndex.json", "w");
  serializeJson(doc, fileW);
  file.close();

  Serial.println(F("\t\t\t\t...reindexed flight logs...finished"));
  return true;
}

bool flightLoggerLFS::writeFile(int flightNbr) {
  Serial.print(F("Write flight log...flightNbr: "));
  Serial.println(flightNbr);

  char flightName [15];
  sprintf(flightName, "/flight%i.json", flightNbr);

  DynamicJsonDocument doc(4096);
  char json[] = "{}";
  deserializeJson(doc, json);
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
#ifdef DEBUG
  Serial.println(F("writeFile...data"));
  serializeJson(flightLog, Serial);
#endif

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
#ifdef DEBUG
  Serial.println(F("\nwriteFile...traces"));
  serializeJson(flightLog, Serial);
  Serial.println(F(""));
#endif

  // Serial.println(String(doc)));
  File file = LittleFS.open(flightName, "w");
  serializeJson(doc, file);
  file.close();

  Serial.println(F("...flight log write...finished"));
  return true;
}

bool flightLoggerLFS::writeFlightCurrent() {
  return writeFile(geFlightNbrLast() + 1);
}