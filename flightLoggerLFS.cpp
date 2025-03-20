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
    Serial.println(F("...failed to open the root directory"));
    return false;
  }

  // Iterate over all files in the root directory
  File file = root.openNextFile();
  while (file) {
    String fileName = file.name();
    Serial.print(F("\tFound file '"));
    Serial.print(fileName);
    Serial.print(F("'...."));

    // Check if the filename matches the pattern "flight<number>.json" or "flightIndex.json"
    if (!(fileName.startsWith("flight") && fileName.endsWith(".json"))) {
// #if DEBUG
    Serial.println(F("...not valid flight log file."));
// #endif
      // Move to the next file
      file = root.openNextFile();
      continue;
    }
// #if DEBUG
    Serial.print(F("...valid flight log file"));
// #endif

    // Close the file so that it can be erased
    file.close();

    // char fileName2[100] = "/";
    // strcat(fileName2, fileName);
    // Serial.print(F(" '"));
    // Serial.print(fileName2);
    // Serial.print(F("' "));
    // if (LittleFS.remove(file.name())) {
    if (LittleFS.remove("/" + fileName)) 
      Serial.println(F(" and erased successfully."));
    else
      Serial.println(F("and erased failed ro erase."));

    // Move to the next file
    file = root.openNextFile();
  }

  Serial.println(F("...all files cleared."));

  reindexFlights();
  
  return true;
}

bool flightLoggerLFS::eraseLast() {
  bool success = false;
  // Delete the current file
  char flightName [15];
  // TODO: base on json flight log index...
  int number = (int)geFlightNbrsLast();
  sprintf(flightName, "/flight%i", number);
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("eraseLast.fileName: "));
  Serial.println(flightName);
#endif
  File file = LittleFS.open(flightName, "r");
  if (!file) {
    Serial.print(F("Flight log #"));
    Serial.print(number);
    Serial.println(F(" does not exist"));
    return false;
  }

  if (!LittleFS.remove(flightName)) {
    Serial.print(F("Failed to delete flight #"));
    Serial.println(number);
    return false;
  }

  Serial.println(F("Flight deleted successfully"));
  reindexFlights();
  return true;
}

bool flightLoggerLFS::exists(int flightNbr) {
  char flightName [15];
  sprintf(flightName, "/flight%i.json", flightNbr);
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("existsFlight..."));
  Serial.println(flightName);
#endif

  File file = LittleFS.open(flightName, "r");
  if (!file) 
    return false;
  return true;
}

bool flightLoggerLFS::initFileSystem() {
  Serial.println(F("\tInitialize flightLogger file system..."));

  Serial.println(F("\t...flightLogger file system initialized."));
  return true;
}

bool flightLoggerLFS::listAsJson(JsonArray flightLogs) {
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.println(F("Retrieving flight log index..."));
#endif
  
  File file = LittleFS.open("/flightIndex.json");
  if (!file || file.isDirectory()) {
    Serial.println(F("\t...failed to open the flight index"));
    return false;
  }

  JsonDocument doc;
  deserializeJson(doc, file);

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.println(F("\tlistAsJson"));
  serializeJson(doc, Serial);
  Serial.println(F(""));
#endif

  JsonArray array = doc.as<JsonArray>();
  for (JsonObject obj : array) {
    // JsonObject flightLog = flightLogs.createNestedObject();
    // // JsonObject flightLog = flightLog.add<JsonObject>();
    // flightLog["number"] = obj["number"];
    // flightLog["epochS"] = obj["epochS"];
    // flightLogs.add(flightLog);
    flightLogs.add(obj);
  }

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.println(F("\tlistAsJson2"));
  serializeJson(flightLogs, Serial);
  Serial.println();
#endif

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.println(F("...flight log index retrieval...finished"));
#endif
  return true;
}

bool flightLoggerLFS::readFile(int flightNbr) {
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("readFile...lfs...#"));
  Serial.println(flightNbr);
#endif

  char flightName [15];
  sprintf(flightName, "/flight%i.json", flightNbr);
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.println(flightName);
#endif

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("...readFile...lfs...#"));
  Serial.print(flightNbr);
  Serial.println(F("...opening file..."));
#endif
  File file = LittleFS.open(flightName, "r");
  if (!file) {
    Serial.print(F("Flight log #"));
    Serial.print(flightNbr);
    Serial.println(F(" not found"));
    return false;
  }

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("...readFile...lfs...#"));
  Serial.print(flightNbr);
  Serial.println(F("...reading json..."));
#endif
  DynamicJsonDocument doc(4096);
  deserializeJson(doc, file);
  file.close();

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("...readFile...lfs...#"));
  Serial.print(flightNbr);
  Serial.println(F("...processing json..."));
#endif
  JsonObject flightLog = doc.as<JsonObject>();
  if (flightLog.isNull()) {
    Serial.print(F("Flight log #"));
    Serial.print(flightNbr);
    Serial.println(F(" is null"));
    return false;
  }
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  serializeJson(flightLog, Serial);
  Serial.println(F(""));
#endif

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("...readFile...lfs...#"));
  Serial.print(flightNbr);
  Serial.println(F("...free memory..."));
#endif
  if (_flightDataTrace != nullptr)
    free(_flightDataTrace);

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("...readFile...lfs...#"));
  Serial.print(flightNbr);
  Serial.println(F("...get traces..."));
#endif
  JsonArray traces = (flightLog["traces"]).as<JsonArray>();
  _flightDataTraceIndex = traces.size();
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("...readFile...lfs...#"));
  Serial.print(flightNbr);
  Serial.print(F("..._flightDataTraceIndex: "));
  Serial.println(_flightDataTraceIndex);
#endif
  _flightDataTrace = (flightDataTraceStruct*)malloc(sizeof(flightDataTraceStruct) * _flightDataTraceIndex);
  if (!_flightDataTrace) {
    Serial.print(F("Flight log #"));
    Serial.print(flightNbr);
    Serial.println(F(" failed to reserve memory"));
    return false;
  }
    
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("...readFile...lfs...#"));
  Serial.print(flightNbr);
  Serial.println(F("...set data..."));
#endif
  _flightData.epochS = flightLog["epochS"];
  _flightData.altitudeApogee = flightLog["altitudeApogee"];
  _flightData.altitudeInitial = flightLog["altitudeInitial"];
  _flightData.altitudeLaunch = flightLog["altitudeLaunch"];
  _flightData.altitudeTouchdown = flightLog["altitudeTouchdown"];
  _flightData.timestampLaunch = flightLog["timestampLaunch"];
  _flightData.timestampApogee = flightLog["timestampApogee"];
  _flightData.timestampTouchdown = flightLog["timestampTouchdown"];

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("...readFile...lfs...#"));
  Serial.print(flightNbr);
  Serial.println(F("...set traces..."));
#endif
  long index = 0;
  for (JsonVariant record : traces) {
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
    debug("-------", "-------");
    debug("index", index);
    debug("diffTime", (unsigned long)record["diffTime"]);
    debug("accelX", (float)record["accelX"]);
    debug("accelY", (float)record["accelY"]);
    debug("accelZ", (float)record["accelZ"]);
    debug("altitude", (float)record["altitude"]);
    debug("gyroX", (float)record["gyroX"]);
    debug("gyroY", (float)record["gyroY"]);
    debug("gyroZ", (float)record["gyroZ"]);
    debug("humidity", (float)record["humidity"]);
    debug("pressure", (float)record["pressure"]);
    debug("temperature", (float)record["temperature"]);
    debug("velocity",(float)record["velocity"]);
#endif
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

#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.print(F("...readFile...lfs...#"));
  Serial.print(flightNbr);
  Serial.println(F("...finished"));
#endif
  return true;
}

bool flightLoggerLFS::readFileAsJson(int flightNbr, JsonObject object) {
  char flightName [15];
  sprintf(flightName, "/flight%i.json", flightNbr);
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.println(flightName);
#endif

  File file = LittleFS.open(flightName, "r");
  if (!file)
    return false;

  DynamicJsonDocument doc(4096);
  deserializeJson(doc, file);
  file.close();
  // JsonObject root = doc.as<JsonObject>();
  // for (JsonPair kv : root) {
  //   object.set(kv.key(), kv.value());
  // }
  object.set(doc.as<JsonObject>());

  return true;
}

int flightLoggerLFS::reindexFlights() {
  Serial.println(F("\t\t\t\tReindex flight logs index..."));
  
  _flightNumbersLast = 0;

  // Open the root directory
  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) {
    Serial.println(F("\t\t\t\tFailed to open the root directory"));
    return false;
  }
  
  JsonDocument doc;
  deserializeJson(doc, "[]");
  JsonArray flightLogs = doc.to<JsonArray>();

  DynamicJsonDocument doc2(4096);
  // Iterate over all files in the root directory
  File file = root.openNextFile();
  while (file) {
    String fileName = file.name();
// #if defined(DEBUG) || defined(DEBUG_LOGGER)
//     Serial.print(F("\t\t\t\t\tFound file: "));
//     Serial.println(fileName);
// #endif
    // Check if the filename matches the pattern "flight<number>.json"
    if (!(fileName.startsWith("flight") && fileName.endsWith(".json")) || fileName.startsWith("flightIndex")) {
      Serial.print(F("\t\t\t\t\tFound file: "));
      Serial.println(fileName);
      // Move to the next file
      file = root.openNextFile();
      continue;
    }

    Serial.print(F("\t\t\t\t\tFlight log: "));
    Serial.print(fileName);
    Serial.println(F(""));

    deserializeJson(doc2, file);

    JsonObject flightLog = flightLogs.createNestedObject();
    flightLog["number"] = doc2["number"];
    flightLog["epochS"] = doc2["epochS"];

    // Move to the next file
    file = root.openNextFile();
  }

// #if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.println(F("\t\t\t\t\t"));
  serializeJson(doc, Serial);
  Serial.println(F(""));
// #endif

  if (_flightNumbers)
    delete [] _flightNumbers;
  
  _flightNumbersSize = flightLogs.size();
  _flightNumbers = new int[_flightNumbersSize];

  int index = 0;
  for (JsonObject obj : flightLogs) {
    int number = obj["number"];
    _flightNumbers[index] = number;

    if (number > _flightNumbersLast)
      _flightNumbersLast = number;
    index++;
  }
  Serial.print(F("\t\t\t\t\tLast Flight Log Number: "));
  Serial.println(_flightNumbersLast);

  File fileW = LittleFS.open("/flightIndex.json", "w");
  serializeJson(doc, fileW);
  file.close();

  Serial.println(F("\t\t\t\t...reindexing flight logs...finished"));
  return true;
}

bool flightLoggerLFS::writeFile(int flightNbr) {
  Serial.print(F("Write flight log...flightNbr: "));
  Serial.println(flightNbr);

  char flightName [15];
  sprintf(flightName, "/flight%i.json", flightNbr);

  JsonDocument doc;
  JsonObject flightLog = doc.to<JsonObject>();
  flightLog["number"] = flightNbr;
  flightLog["epochS"] = _flightData.epochS;
  flightLog["altitudeApogee"] = _flightData.altitudeApogee;
  flightLog["altitudeInitial"] = _flightData.altitudeInitial;
  flightLog["altitudeLaunch"] = _flightData.altitudeLaunch;
  flightLog["altitudeTouchdown"] = _flightData.altitudeTouchdown;
  flightLog["timestampLaunch"] = _flightData.timestampLaunch;
  flightLog["timestampApogee"] = _flightData.timestampApogee;
  flightLog["timestampTouchdown"] = _flightData.timestampTouchdown;
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
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
#if defined(DEBUG) && defined(DEBUG_FLIGHT_LOGGER)
  Serial.println(F("\nwriteFile...traces"));
  serializeJson(flightLog, Serial);
  Serial.println(F(""));
#endif

  // Serial.println(String(doc)));
  File file = LittleFS.open(flightName, "w");
  serializeJson(doc, file);
  file.close();

  reindexFlights();

  Serial.println(F("...flight log write...finished"));
  return true;
}

bool flightLoggerLFS::writeFlightCurrent() {
  return writeFile(geFlightNbrsLast() + 1);
}