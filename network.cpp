// #include "ble.h"
#include "network.h"
#include "wifi.h"
#include "web.h"

void networkDisable() {
  // _ble.disable();
  _wifi.disable();
  _web.disable();
}

void networkLoop() {
  // _ble.loop();
  _wifi.loop();
  _web.loop();
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