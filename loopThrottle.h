#ifndef _LOOP_THROTTLE_H
#define _LOOP_THROTTLE_H

class loopThrottle {
  public:
    loopThrottle();
    int determine(unsigned long timestamp, byte sampleRate);
    void reset();
    
  private:
    int _count;
    int _timestamp;
};

#endif