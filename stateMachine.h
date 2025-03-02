#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

#include "loopThrottle.h"

#define PREFERENCE_KEY_ALTITUDE_AIRBORNE_ASCENT "smSRAA"
#define PREFERENCE_KEY_ALTITUDE_AIRBORNE_DESCENT "smSRAD"
#define PREFERENCE_KEY_ALTITUDE_GROUND "smSRG"
#define PREFERENCE_KEY_ALTITUDE_LIFTOFF "smAL"
#define PREFERENCE_KEY_LAUNCH_DETECT "smLD"

enum loopStates {
  ABORTED,
  AIRBORNE_ASCENT,
  AIRBORNE_DESCENT,
  GROUND,
  LANDED
};

struct loopStateMachine {
  enum loopStates current = GROUND;
};

class stateMachine {
  public:
    stateMachine();
    void loop(unsigned long timestamp, unsigned long delta);
    void reset();
    void save(int launchDetect, int sampleRateAirborneAscent, int sampleRateAirborneDecent, int sampleRateGround);
    void setup();

    int launchDetect();
    int sampleRateAirborneAscent();
    int sampleRateAirborneDescent();
    int sampleRateGround();

    int launchDetectValues[10] = { 5, 10, 15, 20, 25, 30, 35, 40, 45, 50 };
    int sampleRateAirborneAscentValues[4] = { 15, 20, 25, 30 };
    int sampleRateAirborneDecentValues[8] = { 1, 2, 3, 4, 5, 6, 8, 10 };
    int sampleRateGroundValues[8] = { 5, 10, 15, 20, 25, 30, 35, 40 };
    
  private:
    void loopStateABORTED(unsigned long timestamp, unsigned long deltaElapsed);
    void loopStateABORTEDToGROUND(unsigned long timestamp);
    void loopStateAIRBORNEToAbort(char message1[], char message2[]);
    float loopStateAIRBORNE(unsigned long currentTimestamp, long diffTime);
    void loopStateAIRBORNE_ASCENT(unsigned long timestamp, unsigned long deltaElapsed);
    void loopStateAIRBORNE_ASCENTToAIRBORNE_DESCENT();
    void loopStateAIRBORNE_DESCENT(unsigned long timestamp, unsigned long deltaElapsed);
    void loopStateAIRBORNE_DESCENTToLANDED();
    void loopStateLANDED(unsigned long timestamp, unsigned long deltaElapsed);
    void loopStateLANDEDToGROUND();
    void loopStateToGROUND();
    void loopStateGROUND(unsigned long timestamp, unsigned long deltaElapsed);
    void loopStateGROUNDToAIRBORNE_ASCENT(unsigned long timestamp);
    int _checkValues(int values[], int value, int defaultValue, int size);
    void _displaySettings();

    int _altitudeLiftoff = 20;
    int _altitudeGround = 0;
    int _countdownAborted = 0;
    int _countdownLanded = 0;
    loopStateMachine _loopState;
    loopThrottle _throttleAborted;
    loopThrottle _throttleAirborneAscent;
    loopThrottle _throttleAirborneDescent;
    loopThrottle _throttleLanded;
    loopThrottle _throttleGround;
    int _sampleRateAirborneAscent;
    int _sampleRateAirborneDescent;
    int _sampleRateGround;

    // Assumed environmental values
    float altitudeBarometer = 1650.3;  // meters ... map readings + barometer position
    int timeoutRecording = 300 * 10000;
    int timeOutTimeToApogee = 20000;
};

extern stateMachine _stateMachine;

#endif