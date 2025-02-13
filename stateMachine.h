#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

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

    int _countdownAborted = 0;
    int _countdownLanded = 0;
    loopStateMachine _loopState;
    loopThrottle _throttleAborted;
    loopThrottle _throttleAirborneAscent;
    loopThrottle _throttleAirborneDescent;
    loopThrottle _throttleLanded;
    loopThrottle _throttleGround;
};

extern stateMachine _stateMachine;

#endif