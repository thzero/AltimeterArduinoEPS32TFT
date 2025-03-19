// #include "ble.h"
#include "LoRa.h"
#include "network.h"
#include "web.h"
#include "wifi.h"

void networkDisable() {
  // _ble.disable();
  _wifi.disable();
  _web.disable();
}

bool networkEnabled() {
  // return _ble.enabled();
  return _wifi.enabled();
}

void networkLoop() {
  // _ble.loop();
  // _wifi.loop();
  // _web.loop();
}

void networkStart() {
  // _ble.start();
  _wifi.start();
  _web.start();
}

void setupNetwork() {
  // _ble.setup();
  _wifi.setup();
  _web.setup();
}