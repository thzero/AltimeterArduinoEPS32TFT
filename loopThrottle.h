#ifndef _LOOP_THROTTLE_H
#define _LOOP_THROTTLE_H

class loopThrottle {
  public:
    loopThrottle();
    int determine(unsigned long timestamp, byte sampleRate);
    void reset();
    bool signal();
    
  private:
    int _countDebug;
// #ifdef DEBUG
    // int _countDebugI;
// #endif
    int _delta;
};

#endif