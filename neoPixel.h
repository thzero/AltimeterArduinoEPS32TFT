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
extern void setupNeoPixelBlinker();
extern void setupNeoPixelBlinkerDeninit();
extern void setupNeoPixel(Adafruit_NeoPixel pixels);
extern void setupNeoPixel(Adafruit_NeoPixel pixels, int brightness);
extern void turnOfNeoPixel(Adafruit_NeoPixel pixels);
extern void turnOnNeoPixel(Adafruit_NeoPixel pixels, int hex);

#endif