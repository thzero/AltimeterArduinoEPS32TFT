#ifndef _WEB_H
#define _WEB_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>

#include "ESPAsyncWebServer.h"

class web {
  public:
    web();
    void disable();
    void loop();
    void setup();
    void start();

  protected:
    void eraseFlightLogs();
    
  private:
    bool _eraseFlightTaskStatus;
    TaskHandle_t _eraseFlightsTaskHandle;
    int _eraseFlightType;
    File _fileLittleFS;
    AsyncWebServer *_server = NULL;

    void configure();
    void serverHandleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
    void serverHandleUploadLittleFS(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
    void serverHandleUploadOTAUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
    void serverNotFound(AsyncWebServerRequest *request);
    // String serverProcessor(const String& var);
    void jsonAtmosphere(JsonObject root);
    void jsonHeader(JsonObject root);
    void jsonLaunch(JsonObject root, bool settings);
    void jsonMonitor(JsonObject root);
    void jsonSamples(JsonObject root, bool settings);
    void jsonWifi(JsonObject root);
    int _chunkedRead(File file, uint8_t* buffer, size_t maxLen, size_t index);
    void _values(char *name, int values[], JsonArray valuesJson, int size);

    static void eraseFlightLogsTaskW(void * parameter);
};

extern web _web;

#endif