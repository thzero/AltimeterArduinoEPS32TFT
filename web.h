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
    
  private:
    AsyncWebServer *_server = NULL;
    File fileLittleFS;

    void configure();
    void feedWatchdog();
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
};

extern web _web;

#endif