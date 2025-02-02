#include "debug.h"
#include "loopThrottle.h"

loopThrottle::loopThrottle() {
}

int loopThrottle::determine(unsigned long timestamp, byte sampleRate) {
  unsigned long delta = timestamp - _timestamp;
  int samples = ((float)sampleRate / 60) * 1000;
  
// #ifdef DEBUG
  // if (_count % 100000  == 0) {
    // debug(F("timestamp"), timestamp);
    // debug(F("_timestamp"), _timestamp);
    // debug(F("delta"), delta);
    // debug(F("sampleRate"), sampleRate);
    // debug(F("samples"), ((float)sampleRate / 60));
    // debug(F("samples"), samples);
    // debug(F("(delta <= samples)"), (delta <= samples));
    // _count = 0;
  // }
  // _count++;
// #endif

  if (delta <= samples)
    return 0;

  _timestamp = timestamp;
  return delta;
};

void loopThrottle::reset() {
  _count = 0;
  _timestamp = 0;
};