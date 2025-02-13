#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define BOARD_NAME "AltimeterEPS32TFT"
#define BOARD_FIRMWARE "AltimeterEPS32TFT"

#define COPYRIGHT "(C) thZero.com"
#define COPYRIGHT_YEARS "2024-2024"

#define MAJOR_VERSION 0
#define MINOR_VERSION 1

//////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////

// #define SERIAL_BAUD 38400
#define SERIAL_BAUD 115200
#define STATUS_HEIGHT_BAR 10

// Sample rate is in Hz
// Based on Eggtimer Quantum documentation
// https://eggtimerrocketry.com/wp-content/uploads/2024/02/Eggtimer-Quantum-1_09G.pdf, page 25
#define SAMPLE_RATE_ABORTED 5
#define SAMPLE_RATE_AIRBORNE_ASCENT 20
#define SAMPLE_RATE_AIRBORNE_DESCENT 2
#define SAMPLE_RATE_GROUND 10
#define SAMPLE_RATE_LANDED 5
#define SAMPLE_MEASURES_ABORTED 20
#define SAMPLE_MEASURES_APOGEE 5
#define SAMPLE_MEASURES_LANDED 20

#define SAMPLE_RATE_MEMORY 1

// #define USE_SLEEP_MODE

// #define KALMAN
// #define KALMAN_ALTITUDE
// #define KALMAN_PRESSURE
// #define KALMAN_TEMPERATURE

#define DEBUG
#define DEBUG_INTERNAL
// #define DEBUG_ALTIMETER
// #define DEBUG_SENSOR
// #define DEBUG_SIM

#define DEV
#define DEV_SIM

#endif