#ifndef _WEB_H
#define _WEB_H

#include <Arduino.h>

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

    void configure();
    void serverNotFound(AsyncWebServerRequest *request);
};

extern web _web;

#endif