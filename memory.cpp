#include "debug.h"
#include "memory.h"

memory::memory() {
}

uint32_t memory::heap() {
  return _heap;
}

uint32_t memory::heapInternal() {
 return _heap_internal;
}

uint32_t memory::heapMinimum() {
 return _heap_minimum;
}

void memory::loop() {
  _heap = _heap_filter.filter(esp_get_free_heap_size() / 1000);
  _heap_internal = _heap_filter.filter(esp_get_free_internal_heap_size() / 1000);
  _heap_minimum = _heap_filter.filter(esp_get_minimum_free_heap_size() / 1000);
}

void memory::setup() {
  _heap = _heap_filter.filter(esp_get_free_heap_size() / 1000);
  _heap_internal = _heap_filter.filter(esp_get_free_internal_heap_size() / 1000);
  _heap_minimum = _heap_filter.filter(esp_get_minimum_free_heap_size() / 1000);
}

memory _memory;