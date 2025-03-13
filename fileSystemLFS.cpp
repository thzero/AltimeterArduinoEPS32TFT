#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include "constants.h"
#include "debug.h"
#include "fileSystemLFS.h"
#include "utilities.h"

fileSystemLFS::fileSystemLFS() {
}

void fileSystemLFS::loadConfigSim(JsonArray configs) {
// #ifdef DEBUG_SIM
//  Serial.println(F("\nLoad config - simulation..."));
// #endif

  // Open the root directory
  File file = LittleFS.open("/sim.json");
  if (!file || file.isDirectory()) {
    Serial.println(F("...failed to open the sim config."));
    // results.success = false;
    // return results;
    return;
  }

  DynamicJsonDocument doc(4096);
  deserializeJson(doc, file);
// #ifdef DEBUG_SIM
  // serializeJson(doc, Serial);
  // Serial.println();
// #endif
  JsonArray array = doc.as<JsonArray>();
  for (JsonObject obj : array) {
// #ifdef DEBUG_SIM
    // Serial.println("obj...");
    // serializeJson(obj, Serial);
    // Serial.println();
// #endif
    configs.add(obj);
// #ifdef DEBUG_SIM
    // Serial.println("configs...obj...");
    // serializeJson(configs, Serial);
// #endif
  }
  
// #ifdef DEBUG_SIM
  // Serial.println("configs...");
  // serializeJson(configs, Serial);
  // Serial.println();
// #endif
// #ifdef DEBUG_SIM
  // Serial.println(F("...config - simulation - load successful."));
// #endif
}

bool fileSystemLFS::setup() {
  Serial.println(F("\nSetup file system..."));

  if (!LittleFS.begin(false)) {
    Serial.println(F("\tLittleFS mount failed"));
    Serial.println(F("\tDid not find filesystem; starting format"));

    // format if begin fails
    if (!LittleFS.begin(true)) {
#if defined(DEBUG) || defined(DEBUG_LOGGER)
      Serial.println(F("\tLittleFS mount failed"));
      Serial.println(F("\tFormatting not possible"));
#endif

      Serial.println(F("...file system setup failed."));
      return false;
    }

    Serial.println(F("Formatting"));
  }
  
  Serial.println(F("...file system setup successful."));
  return true;
}

long fileSystemLFS::totalBytes() {
  return LittleFS.totalBytes();
}

long fileSystemLFS::usedBytes() {
  return LittleFS.usedBytes();
}