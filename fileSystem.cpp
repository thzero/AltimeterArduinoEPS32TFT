#include <Arduino.h>

#include "constants.h"
#include "fileSystem.h"
#include "fileSystemBase.h"

fileSystem::fileSystem() {
}

bool fileSystem::setup() {
  instance.setup();
}

long fileSystem::totalBytes() {
  return instance.setup();
}

long fileSystem::usedBytes() {
  return instance.setup();
}

fileSystem _fileSystem;

void setupFileSystem() {
  Serial.println(F("\nSetup file system..."));

  if (!_fileSystem.instance.setup()) {
    Serial.println(F("Failed to initialize file system"));
    return;
  }

  Serial.println(F("...file system successful."));
}