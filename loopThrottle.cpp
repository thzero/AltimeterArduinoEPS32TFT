#include "debug.h"
#include "loopThrottle.h"

loopThrottle::loopThrottle() {
}

int loopThrottle::determine(unsigned long deltaIn, byte sampleRate) {
  unsigned long delta = deltaIn + _delta;
  int samples = ((float)sampleRate / 60) * 1000;
  
// #ifdef DEBUG
  // if (_count % 100000 == 0) {
  //   debug(F(" "));
  //   debug(F("deltaIn"), deltaIn);
  //   debug(F("_delta"), _delta);
  //   debug(F("delta"), delta);
  //   debug(F("sampleRate"), sampleRate);
  //   debug(F("samples"), ((float)sampleRate / 60));
  //   debug(F("samples"), samples);
  //   debug(F("(delta <= samples)"), (delta <= samples));
  //   _count = 0;
  // }
  // _count++;
// #endif

  if (delta <= samples) {
    _delta = delta;
    return 0;
  }

// #ifdef DEBUG
  // debug(F("...process"));
// #endif
  _delta = 0;
  return delta;
};

void loopThrottle::reset() {
  _count = 0;
  _delta = 0;
};