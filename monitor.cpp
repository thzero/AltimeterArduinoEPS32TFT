#include <Arduino.h>

#include "constants.h"
#include "debug.h"
#include "monitor.h"

monitor::monitor() {
}

uint32_t monitor::heap() {
  return _heap;
}

uint32_t monitor::heapInternal() {
 return _heap_internal;
}

uint32_t monitor::heapMinimum() {
 return _heap_minimum;
}

void monitor::loop() {
#ifdef MONITOR_MEMORY
  _heap = _heap_filter.filter(esp_get_free_heap_size() / 1000);
  _heap_internal = _heap_filter.filter(esp_get_free_internal_heap_size() / 1000);
  _heap_minimum = _heap_filter.filter(esp_get_minimum_free_heap_size() / 1000);
#endif

#ifdef MONITOR_VOLTAGE
  // Voltage in volts not mv, using a 1:1 divider so 500 division.
  _voltage = _voltage_filter.filter(analogReadMilliVolts(10) / 500);

  memmove(&_voltageRolling[0], &_voltageRolling[1], (_voltageCapacity -1) * sizeof(_voltageRolling[0]));
  _voltageRolling[_voltageCapacity - 1] = analogReadMilliVolts(10) / 500;
#endif
}

void monitor::setup() {
#ifdef MONITOR_MEMORY
  _heap = _heap_filter.filter(esp_get_free_heap_size() / 1000);
  _heap_internal = _heap_filter.filter(esp_get_free_internal_heap_size() / 1000);
  _heap_minimum = _heap_filter.filter(esp_get_minimum_free_heap_size() / 1000);
#endif

#ifdef MONITOR_VOLTAGE
  // It turns out that pins using ADC2 instead of ADC1 can conflict with the WiFi so it's good practice to use a ACD1 port.
  // Also if you use the analogReadMilliVolts(PIN) option instead of working it out yourself it is calibrated for the fact the pin read is not fully linear and gives a nice exact voltage.

  // Put voltage divider down the VBAT side which is easier from VBAT to pin10, and then the second resistor from pin10 to pin 6.
  // Just set pin6 to a LOW output and pin10 to an input.  Normally go for more than 33k's
  analogReadResolution(12);
  pinMode(10, INPUT);   
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  
  _voltage = _voltage_filter.filter(analogReadMilliVolts(10) / 500);
#endif
}

uint32_t monitor::voltage() {
 return _voltage;
}

monitor _monitor;