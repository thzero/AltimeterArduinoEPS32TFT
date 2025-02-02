#include "sensorData.h"

// Assumed environmental values
float pressureReference = 1018.6; // hPa local QFF (official meteor-station reading)
float temperatureOutdoor = 4.7; // °C  measured local outdoor temp

bool _initialized;