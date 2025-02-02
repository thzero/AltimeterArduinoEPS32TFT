#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "ble.h"
#include "constants.h"
#include "debug.h"

//////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

ble::ble() {
}

bool ble::connected() {
  return false; // TODO
}

void ble::disable() {
  Serial.println(F("Disable network BLE..."));

  BLEDevice::deinit(true);

  Serial.println(F("...network BLE disabled successful."));
}

bool ble::enabled() {
  return false; // TODO
}

void ble::loop() {
}

void ble::start() {
  Serial.println(F("Start network BLE..."));

  BLEDevice::init(BOARD_NAME);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID,BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println(F("Characteristic defined! Now you can read it in your phone!"));

  Serial.println(F("...network BLE successful."));
}

 ble _ble;