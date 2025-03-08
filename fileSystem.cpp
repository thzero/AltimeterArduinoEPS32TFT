#include <LittleFS.h>

#include "constants.h"
#include "flightLogger.h"

bool setupFileSystem() {
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

size_t fileSystemTotalBytes() {
  return LittleFS.totalBytes();
}

size_t fileSystemUsedBytes() {
  return LittleFS.usedBytes();
}