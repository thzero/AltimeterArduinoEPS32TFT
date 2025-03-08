#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include "constants.h"
#include "debug.h"
#include "fileSystemLFS.h"
#include "utilities.h"

fileSystemLFS::fileSystemLFS() {
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