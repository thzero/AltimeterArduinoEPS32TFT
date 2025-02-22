#ifndef _MONITOR_H
#define _MONITOR_H

#include "ewma.h"

class monitor {
  public:
    monitor();
    uint32_t heap();
    uint32_t heapInternal();
    uint32_t heapMinimum();
    void loop();
    void setup();
    uint32_t voltage();
    
  private:
    uint32_t _heap;
    uint32_t _heap_internal;
    uint32_t _heap_minimum;
    uint32_t _voltage;
    uint32_t _voltageRolling[201];
    const size_t _voltageCapacity = 200;

    ewma _heap_filter = ewma(0.01); 
    ewma _heap_internal_filter = ewma(0.01); 
    ewma _heap_minimum_filter = ewma(0.01); 
    ewma _voltage_filter = ewma(0.01); 
};

extern monitor _monitor;

#endif