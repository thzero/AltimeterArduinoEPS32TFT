#ifndef _WIFI_H
#define _WIFI_H

#include <Arduino.h>

class wifi {
  public:
    wifi();
    bool connected();
    void disable();
    bool enabled();
    String ipAddress();
    void loop();
    String ssid();
    void setup();
    void start();
    
  private:
    String _ipAddress;
    String _ssId;
};

extern wifi _wifi;

#endif