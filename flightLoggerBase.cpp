#include "flightLoggerBase.h"

bool flightLoggerBase::clearFlightList() {
  return false;
}

void flightLoggerBase::determineFlightMinAndMax(int flightNbr) {
}

long flightLoggerBase::getFlightDuration() {
  return 0;
}

float flightLoggerBase::getFlightAccelXMax() {
  return 0;
}

float flightLoggerBase::getFlightAccelXMin() {
  return 0;
}

float flightLoggerBase::getFlightAccelYMax() {
  return 0;
}

float flightLoggerBase::getFlightAccelYMin() {
  return 0;
}

float flightLoggerBase::getFlightAccelZMax() {
  return 0;
}

float flightLoggerBase::getFlightAccelZMin() {
  return 0;
}

float flightLoggerBase::getFlightAltitudeMax() {
  return 0;
}

float flightLoggerBase::getFlightAltitudeMin() {
  return 0;
}

flightDataStruct* flightLoggerBase::getFlightData() {
  return nullptr;
}

float flightLoggerBase::getFlightHumidityMax() {
  return 0;
}

float flightLoggerBase::getFlightHumidityMin() {
  return 0;
}

float flightLoggerBase::getFlightPressureMax() {
  return 0;
}

float flightLoggerBase::getFlightPressureMin() {
  return 0;
}

float flightLoggerBase::getFlightTemperatureMax() {
  return 0;
}

float flightLoggerBase::getFlightTemperatureMin() {
  return 0;
}

float flightLoggerBase::getFlightVelocityMax() {
  return 0;
}

float flightLoggerBase::getFlightVelocityMin() {
  return 0;
}

long flightLoggerBase::geFlightNbrLast() {
  return 0;
}

long flightLoggerBase::getFlightSize() {
  return 0;
}

bool flightLoggerBase::initFileSystem() {
  return false;
}

bool flightLoggerBase::initFlight() {
  return false;
}

void flightLoggerBase::printFlightData(int flightNbr) {
}

bool flightLoggerBase::writeFlightFast() {
  return false;
}

void flightLoggerBase::setFlightAccelX(float x) {
}

void flightLoggerBase::setFlightAccelY(float y) {
}

void flightLoggerBase::setFlightAccelZ(float z) {
}

void flightLoggerBase::setFlightAltitude(float altitude) {
}

bool flightLoggerBase::setFlightData() {
  return false;
}

void flightLoggerBase::setFlightGyroX(float x) {
}

void flightLoggerBase::setFlightGyroY(float y) {
}

void flightLoggerBase::setFlightGyroZ(float z) {
}

void flightLoggerBase::setFlightHumidity(float humidity) {
}

void flightLoggerBase::setFlightPressure(float pressure) {
}

void flightLoggerBase::setFlightTemperature(float temperature) {
}

void flightLoggerBase::setFlightTime(long diffTime) {
}

void flightLoggerBase::setFlightVelocity(float velocity) {
}

bool flightLoggerBase::readFlight(int flightNbr) {
  return false;
}

bool flightLoggerBase::writeFlight(int flightNbr) {
  return false;
}
