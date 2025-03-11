#include "debug.h"
#include "loopThrottle.h"

loopThrottle::loopThrottle() {
}

int loopThrottle::determine(unsigned long deltaElapsed, int sampleRate) {
  unsigned long delta = deltaElapsed + _delta;
  int samples = 1000 / sampleRate;
  
// #ifdef DEBUG
  // if (_countDebugI % 100000 == 0) {
  //   debug(F(" "));
  //   debug(F("deltaElapsed"), deltaElapsed);
  //   debug(F("_delta"), _delta);
  //   debug(F("delta"), delta);
  //   debug(F("sampleRate"), sampleRate);
  //   debug(F("samples"), ((float)sampleRate / 60));
  //   debug(F("samples"), samples);
  //   debug(F("(delta <= samples)"), (delta <= samples));
  //   _countDebugI = 0;
  // }
  // _countDebugI++;
// #endif

  if (delta <= samples) {
    _countDebug++;
    _delta = delta;
    return 0;
  }

// #ifdef DEBUG
  // debug(F("...process"));
// #endif
  _countDebug = 0;
  _delta = 0;
  return delta;
};

void loopThrottle::reset() {
// #ifdef DEBUG
  // _countDebugI = 0;
// #endif
  _countDebug = 0;
  _delta = 0;
};

bool loopThrottle::signal() {
  return _countDebug % 100 == 0;
};