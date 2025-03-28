#ifndef _TFT_H
#define _TFT_H

#include <TFT_eSPI.h>

extern TFT_eSPI _tft;

extern void drawTftFlightAirborne(unsigned long timestamp, unsigned long delta);

extern void drawTftFlightAirborneStart();

extern void drawTftGraphAxesXY(float minX, float maxX, float minY, float maxY, int flightNbr, char *curveName);

extern void drawTftGraphForlightNbr(int flightNbr, int curveType);

extern void drawTftReset();

extern void drawTftSensorImu();

extern String drawTftSplashPad(float value, int width, char * format);

extern void drawTftSleep();

extern void drawTftSplash();

extern void drawTftSplashSim();

extern void drawTftSplashSimStop();

extern void setupTft();

extern void sleepTft();

extern void sleepTftHold();

#endif