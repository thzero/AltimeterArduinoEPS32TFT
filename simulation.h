#ifndef _SIMULATION_H
#define _SIMULATION_H

#include "loopThrottle.h";
#include "sensorData.h";

#define SAMPLE_SIMULATION 1

struct simulationConfig {
  double RocketMass = 1.76;                 // kg // from OR 3" rocket
  double MotorFuelBurnRate = 0.176;         // kg/s // from OR 3" rocket
  double MotorFuelBurnTime = 1.6;         // kg/s // from OR 3" rocket
  double MotorFuelMass = 0.316;             // kg // from OR 3" rocket
  double MotorThrust = 2485.796;            // m/s // from OR 3" rocket  
  double CrossSection = 4.56e-3;            // m^20.0762 // from OR 3" rocket
  double DragCoefficient = 0.87;            // from OR 3" rocket
  int sampleRate = 100;
};

// Very simple, and not very realistic, simulation only on the X and Y axis.
class simulation {
  public:
    simulation();
    bool isRunning();
    void outputSerialList();
    void start(long initialAltitude);
    void stop();
    accelerometerValues valueAcceleration();
    double valueAltitude();

  private:
    double AirDensity = 1.292;        // kg/m^3
    double AirDensityScale = 5600.0;  // m -- height at which density is halved
    double EarthMass = 5.97e24;       // kg
    double GravConstant = 6.67e-11;   // m^3/kg/s^2;
    double EarthRadius = 6.3e6;       // m
    int finalCountdown = 100;

    bool _airborne = false;
    double _altitudeInitial;
    double _altitudeStarting;
    double _burnoutTime;  // s
    simulationConfig _config;
    float _elapsedTime;
    double _motorThrust;  // N
    bool _running = false;
    double _trace[3];
    int _finalCountdown;
    
    double _maxAltitude;
    double _maxVelocity;
    unsigned long _simulationTimestamp;
    loopThrottle _simulationThrottle;
    TaskHandle_t simulationTaskHandle;

    void evaluateTimestep(float deltaT, double drag, double Mr, double Me, double G, double Ft, double *x);
    void outputPrint(float delta, double mass, double thrust, double altitude, double airDensity, double drag, bool burnOut);
    void outputPrintHeader();
    void loopStep(float deltaT, bool output);
    void simulationTask();
    static void simulationTaskW(void * parameter);
};

extern simulation _simulation;
extern simulationConfig simulationConfigDefault;

#endif                                                             