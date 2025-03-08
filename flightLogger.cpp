#include "constants.h"
#include "flightLoggerBase.h"
#include "flightLogger.h"

flightLogger::flightLogger() {
  reset();
}

void flightLogger::init(unsigned long timestamp) {
  // Initialize the flight...
  aborted = false;
  recording = true;

  airborne = true;
  touchdown = false;

  instance.init(timestamp);
}

// Kick off reindexing on the other cpu...
void flightLogger::reindexFlights() {
  BaseType_t xReturned = xTaskCreatePinnedToCore(
    &flightLogger::reindexFlightsTaskW, /* Function to implement the task */
    "reindexFlightsTask", /* Name of the task */
    4000,  /* Stack size in words */
    this,  /* Task input parameter */
    0,  /* Priority of the task */
    &reindexFlightsTaskHandle,  /* Task handle. */
    0 /* Core where the task should run */
  );
}

void flightLogger::reindexFlightsTaskW(void * parameter) {
  flightLogger* instance = reinterpret_cast<flightLogger*>(parameter); // obtain the instance pointer
  instance->instance.reindexFlights(); // dispatch to the member function, now that we have an instance pointer
  // Delete the task...
  vTaskDelete(NULL);
}

void flightLogger::reset() {
  // Reset the flight...
  aborted = false;
  recording = false;

  airborne = false;
  touchdown = false;

  instance.reset();
}

flightLogger _flightLogger;

void setupFlightLogger() {
  Serial.println(F("\nSetup flight logger..."));

  if (!_flightLogger.instance.initFileSystem()) {
    Serial.println(F("Failed to initialize file system"));
    return;
  }

  _flightLogger.reindexFlights();

  Serial.println(F("...flight logger successful."));
}