#ifndef _LEDS_H
#define _LEDS_H

class ledsBuiltin {
  public:
    ledsBuiltin();
    void setup();
    void turnOff();
    void turnOn();
};

extern ledsBuiltin _ledsBuiltin;

#endif