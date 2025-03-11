#ifndef _WIFI_H
#define _WIFI_H

#include <Arduino.h>

#define PREFERENCE_KEY_WIFI_ADDRESS "wifiAddress"
#define PREFERENCE_KEY_WIFI_PASSWORD "wifiPassword"
#define PREFERENCE_KEY_WIFI_SSID "wifiSSID"

class wifi {
  public:
    wifi();
    bool connected();
    void disable();
    bool enabled();
    String ipAddress();
    void loop();
    String password();
    void reset();
    void save(const char * password, const char * ssid);
    String ssid();
    void setup();
    void start();
    
  private:
    String _ipAddress;
    String _password;
    String _ssId;
    
    void _determineSsid();
    void _reset();
};

extern wifi _wifi;

#endif