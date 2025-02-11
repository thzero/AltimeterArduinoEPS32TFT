#ifndef _LEDS_H
#define _LEDS_H

class leds {
  public:
    leds();
    void setupBuiltin();
    void turnOffBuiltin();
    void turnOnBuiltin();
};

extern leds _leds;

#endif