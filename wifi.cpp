#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>

#include "constants.h"
#include "debug.h"
#include "wifi.h"

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 4);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);
// IPAddress primaryDNS(8, 8, 8, 8);   // optional
// IPAddress secondaryDNS(8, 8, 4, 4); // optional

wifi::wifi() {
}

bool wifi::connected() {
  return WiFi.status() == WL_CONNECTED;
}

void wifi::disable() {
  Serial.println(F("Disable network WiFi..."));

  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);

  Serial.println(F("...network WiFi disabled successful."));
}

bool wifi::enabled() {
  return WiFi.getMode() != WIFI_OFF;
}

String wifi::ipAddress() {
  return _ipAddress;
}

void wifi::loop() {
  // if (WiFi.status() == WL_CONNECTED) {
  //   Serial.print(F("WiFi connected..."));
  // }
}

String wifi::password() {
  return _password;
}

void wifi::reset() {
  Serial.println(F("Reset network WiFi..."));

  _reset();

  Serial.println(F("...network WiFi reset successful."));
}

void wifi::save(const char * password, const char * ssId) {
  Serial.println(F("Saving network WiFi preferences..."));

  // TODO
  // disable();
  
  // TODO
  // Preferences preferences;
  // preferences.begin(PREFERENCE_KEY, false);
  // _password = password;
  // preferences.putString(PREFERENCE_KEY_WIFI_PASSWORD, _password);
  // _ssId = ssId;
  // preferences.putString(PREFERENCE_KEY_WIFI_SSID, _ssId);
  // preferences.end();

  // TODO
  // start();

  Serial.println(F("...network WiFi preferences saved successful."));
}

String wifi::ssid() {
  return _ssId;
}

void wifi::setup() {
  Serial.println(F("Setup network WiFi..."));

  _reset();

  Serial.println(F("...network WiFi setup successful."));
}

void wifi::start() {
  Serial.println(F("Start network WiFi..."));

  debug("password", _password);
  WiFi.softAP(_ssId, _password);

  // if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println(F("WiFi: STA Failed to configure"));

    Serial.println(F("...network WiFi failed."));
    return;
  }

  _ipAddress = WiFi.softAPIP().toString();
  Serial.println(F(""));
  Serial.print(F("WiFi IP address: "));
  Serial.println(_ipAddress);
  Serial.println(F(""));

  Serial.println(F("...network WiFi started successful."));
}

void wifi::_determineSsid() {
  char macAddress[14];
  snprintf(macAddress, 14, "%llX", ESP.getEfuseMac());
  // debug(F("macAddress"), macAddress); // 
  String macAddressAsStr = macAddress ; // convert from char to String
  // debug(F("macAddressAsStr"), macAddressAsStr); // 
  int strLen = macAddressAsStr.length();
  // debug(F("Length of the string %i characters \r\n"), macAddressAsStr);
  String macAddressF = "";
  String temp;
  int i = 0; while ( i < 6) { 
    temp = macAddressAsStr.substring(strLen - 2 - 2 * i, strLen - 2 * i);
    macAddressF = macAddressF + temp + ":";
    if (i > 2)
      _ssId = _ssId + temp;
    i++ ;
  }
  macAddressF = macAddressF.substring(0,17); // remove last : 
  // Serial.print(F("macAddress="));
  // Serial.println(macAddressF);
  // Serial.print(F("_ssId="));
  // Serial.println(_ssId);
  // String serialnumber = "-" ;
  // int j = 3; 
  // while ( j < 6) {  
  //   serialnumber = serialnumber + ssid1.substring(strLen - 2 - 2 * j, strLen - 2 * j);
  //   j++;
  // }
  // debug(F("serialnumber"), serialnumber);

  Serial.println(F(""));
  Serial.print(F("WiFi SSID: "));
  Serial.println(_ssId);
  Serial.println(F(""));
}

void wifi::_reset() {
  _determineSsid();

  Preferences preferences;
  preferences.begin(PREFERENCE_KEY, false);
  _ssId = preferences.getString(PREFERENCE_KEY_WIFI_SSID, _ssId);
  preferences.putString(PREFERENCE_KEY_WIFI_SSID, _ssId);
  _password = preferences.getString(PREFERENCE_KEY_WIFI_PASSWORD, "password123");
  // _password = "password123";
  preferences.putString(PREFERENCE_KEY_WIFI_PASSWORD, _password);
  preferences.end();
}

wifi _wifi;