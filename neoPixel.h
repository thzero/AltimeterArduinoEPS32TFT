#ifndef _BLINKER_H
#define _BLINKER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class neoPixelBlinker {
  public:
    neoPixelBlinker();
    void init(Adafruit_NeoPixel pixels);
    void init(Adafruit_NeoPixel pixels, int brightness);
    void blink(int timestamp, int delayMs);
    void off();
    void setup();
    void setupDeinit();

  protected:
    int cycle(int hexIn);

  private:
    Adafruit_NeoPixel _pixels;
    int _delayMs;
    int _hex = 0xFF0000;
    unsigned int _timestamp;
    int _count;
};
extern neoPixelBlinker _neoPixelBlinker;

class neoPixel {
  public:
    neoPixel();
    void sleep();
    void sleepHold();
    int cycle(Adafruit_NeoPixel pixels, int hexIn);
    void setup(Adafruit_NeoPixel pixels);
    void setup(Adafruit_NeoPixel pixels, int brightness);
    void turnOff(Adafruit_NeoPixel pixels);
    void turnOn(Adafruit_NeoPixel pixels, int hex);
};

extern neoPixel _neoPixel;

#endif