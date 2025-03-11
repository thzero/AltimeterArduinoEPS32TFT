#ifndef _BLINKER_H
#define _BLINKER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class neoPixelBlinker {
  public:
    neoPixelBlinker();
    void blink(int hexIn);
    void blink(int timestamp, int delayMs);
    void init(Adafruit_NeoPixel pixels);
    void init(Adafruit_NeoPixel pixels, int brightness);
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
    void off(Adafruit_NeoPixel pixels);
    void on(Adafruit_NeoPixel pixels, int hex);
    void setup(Adafruit_NeoPixel pixels);
    void setup(Adafruit_NeoPixel pixels, int brightness);
    void sleep();
    void sleepHold();
};

extern neoPixel _neoPixel;

#endif