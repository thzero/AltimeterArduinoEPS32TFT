#include "neoPixel.h"

//////////////////////////////////////////////////////////////////////
// Global variables
//////////////////////////////////////////////////////////////////////
Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
neoPixelBlinker _neoPixelBlinker;

neoPixelBlinker::neoPixelBlinker() {
}

void neoPixelBlinker::init(Adafruit_NeoPixel pixels) {
  _pixels = pixels;
  _pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  _pixels.setBrightness(5);
  off();
  // _delayMs = delayMs;
}

void neoPixelBlinker::init(Adafruit_NeoPixel pixels, int brightness) {
  _pixels = pixels;
  _pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  _pixels.setBrightness(brightness);
  off();
  // _delayMs = delayMs;
}

void neoPixelBlinker::blink(int timestamp, int delayMs) {
  bool blink = false;

  unsigned int delta = timestamp - _timestamp;
  if (delta > delayMs) {
    blink = true;
    _timestamp = timestamp;
  }

  if (blink) {
    // Serial.print(F("-----"));
    // Serial.println(_count);
    if (_count % 1 == 0) {
      // turnOnLedBuiltin();
      // Serial.print(F("NeoPixel on..."));
      // Serial.println(_hex);
      _hex = cycle(_hex);
    }
    if (_count % 2 == 0) {
      _count = 0;
      // turnOffLedBuiltin();
      off();
      // Serial.println(F("NeoPixel off..."));
    }
    _count++;
  }
};

void neoPixelBlinker::off() {
  _pixels.fill(0x000000);
  _pixels.show();
};

int neoPixelBlinker::cycle(int hexIn) {
  // Serial.println(hexIn);
  if (hexIn != 0xFF000 && hexIn != 0x00FF00 && hexIn != 0x0000FF)
    hexIn = 0xFF0000;
  // Serial.println(hexIn);
  int hex = hexIn;
  if (hexIn == 0xFF0000)
    hex = 0x00FF00;
  else if (hexIn == 0x00FF00)
    hex = 0x0000FF;
  else if (hexIn == 0x0000FF)
    hex = 0xFF0000;
  // Serial.println(hex);
  _pixels.fill(hex);
  _pixels.show();
  return hex;
};

int cycleNeoPixel(Adafruit_NeoPixel pixels, int hexIn) {
#ifdef DEBUG
  Serial.println(hexIn);
#endif
  if (hexIn != 0xFF000 && hexIn != 0x00FF00 && hexIn != 0x0000FF)
    hexIn = 0xFF0000;
#ifdef DEBUG
  Serial.println(hexIn);
#endif

  int hex = hexIn;
  if (hexIn == 0xFF0000)
    hex = 0x00FF00;
  else if (hexIn == 0x00FF00)
    hex = 0x0000FF;
  else if (hexIn == 0x0000FF)
    hex = 0xFF0000;
    
#ifdef DEBUG
  Serial.println(hex);
#endif
  pixels.fill(hex);
  pixels.show();
  return hex;
}

void setupNeoPixel(Adafruit_NeoPixel pixels) {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(25);
  pixels.fill(0x000000);
  pixels.show();
}

void setupNeoPixel(Adafruit_NeoPixel pixels, int brightness) {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(brightness);
}

void setupNeoPixelBlinker() {
  _neoPixelBlinker.init(pixels);
}

void setupNeoPixelBlinkerDeninit() {
  _neoPixelBlinker.off();
}

void turnOfNeoPixel(Adafruit_NeoPixel pixels) {
  pixels.fill(0x000000);
  pixels.show();
}

void turnOnNeoPixel(Adafruit_NeoPixel pixels, int hex) {
  pixels.fill(hex);
  pixels.show();
}
