#ifndef _MEMORY_H
#define _MEMORY_H

#include "ewma.h"

class memory {
  public:
    memory();
    uint32_t heap();
    uint32_t heapInternal();
    uint32_t heapMinimum();
    void loop();
    void setup();
    
  private:
    uint32_t _heap;
    uint32_t _heap_internal;
    uint32_t _heap_minimum;
    ewma _heap_filter = ewma(0.01); 
    ewma _heap_internal_filter = ewma(0.01); 
    ewma _heap_minimum_filter = ewma(0.01); 
};

extern memory _memory;

#endif