#ifndef _BUTTON_H
#define _BUTTON_H

#include <Arduino.h>

//////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////
// Built in button GPIO - adjust for your board
#define BUTTON_GPIO GPIO_NUM_0

extern void handleButtonLoop();
extern void setupButton();
extern void setupButtonDeninit();

#endif