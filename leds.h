#ifndef _LEDS_H
#define _LEDS_H

class ledsBuiltin {
  public:
    ledsBuiltin();
    void setup();
    void off();
    void on();
};

extern ledsBuiltin _ledsBuiltin;

#endif