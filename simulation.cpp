#include <Arduino.h>

#include "debug.h"
#include "simulation.h"

struct simulationConfig simulationConfigDefault;

simulation::simulation() {
}

bool simulation::isRunning() {
  return _running;
}

void simulation::evaluateTimestep(double K, double deltaT, double Mr, double Me, double G, double Ft, double *x) {
  // Leap-frog Euler method -- interleave calculation of DeltaX, DeltaV
  // Move the position
  _trace[0] += deltaT * _trace[1];
  // Compute the total force
  double F = -G * Me * Mr / (_trace[0] * _trace[0]);
  double DragSign = (_trace[1] < 0) ? 1.0 : -1.0;
  F += 0.5 * K * (_trace[1] * _trace[1]) * DragSign;
  F += Ft;
  // Apply acceleration
  _trace[1] += deltaT * F / Mr;
}

void simulation::outputPrint(double delta, double thrust, double mass, double height) {
  Serial.print(F("sim -\t"));
  Serial.print(_elapsedTime);
  Serial.print(F("\t"));
  Serial.print(delta);
  Serial.print(F("\t"));
  Serial.print(thrust);
  Serial.print(F("\t"));
  Serial.print(mass);
  Serial.print(F("\t"));
  Serial.print(_trace[1]);
  Serial.print(F("\t\t"));
  Serial.print(_startingAltitude);
  Serial.print(F("\t\t"));
  Serial.print(_trace[0]);
  Serial.print(F("\t"));
  Serial.print(height);
  Serial.print(F("\t"));
  Serial.println(_airborne);
}

void simulation::loopStep(double deltaT) {
  deltaT = deltaT / 1000;

  if (_airborne && _trace[0] <= _startingAltitude) {
    outputPrint(deltaT, 0, 0, 0);
    Serial.println(F("sim -\tLANDED"));
    Serial.print(F("sim -\tMax. Height="));
    Serial.println(_maxHeight);
    Serial.print(F("sim -\tMax. Velocity="));
    Serial.println(_maxVelocity);
    Serial.println(F(""));
    _running = false;
    return;
  }

  double height = _trace[0] - _startingAltitude;
  double airDensity = AirDensity * pow(0.5, height / AirDensityScale);
  double drag = airDensity * _config.CrossSection * _config.DragCoefficient;

  double mass = _config.RocketMass;
  double thrust = 0;
  if (_elapsedTime < _burnoutTime) {
    mass = _config.RocketMass + _config.MotorFuelMass - _config.MotorFuelBurnRate * _elapsedTime;
    thrust = _motorThrust; // N
  }

  outputPrint(deltaT, thrust, mass, height);

  evaluateTimestep(drag, deltaT, mass, EarthMass, GravConstant, thrust, _trace);
  // TODO: Introduce some  point - recommendation seems to be to use Simplex noise from FastLED

  if (_trace[0] > _startingAltitude)
    _airborne = true;

  if (height > _maxHeight)
    _maxHeight = height;
  if (_trace[1] > _maxVelocity)
    _maxVelocity = _trace[1];

  _elapsedTime += deltaT;
}

void simulation::simulationTaskW(void * parameter) {
    simulation* instance = reinterpret_cast<simulation*>(parameter) ; // obtain the instance pointer
    instance->simulationTask() ; // dispatch to the member function, now that we have an instance pointer
}

void simulation::simulationTask() {
  _simulationTimestamp = millis();
  // debug(F("Simulation task..."), _running);
  int settle = 0;
  while(_running) {
    // debug(F("Simulation _simulationTimestamp...", _simulationTimestamp);
    unsigned long current = millis();
    // debug(F("Simulation current..."), current);
    // debug(F("Simulation SAMPLE_SIMULATION..."), SAMPLE_SIMULATION);

    // unsigned long deltaT = current - _simulationTimestamp;
    // debug(F("Simulation deltaT..."), deltaT);

    int delta = _simulationThrottle.determine(_simulationTimestamp, _config.sampleRate);
    // debug(F("Simulation delta..."), delta);
    if (delta != 0) {
      // debug(F("Simulation settle..."), settle);
      // let the loop settle before starting to simulate...
      if (settle < 5) {
        settle++;
        continue;
      }

      // debug(F("Simulation sim!!!"), delta);
      loopStep(delta);
    }

    _simulationTimestamp = current;
  }
  // Delete the task...
  vTaskDelete(NULL);
}

void simulation::start(simulationConfig startConfig, long initialAltitude) {
  _config = startConfig;
  Serial.println(F(""));
  Serial.println(F("Simulation\tStarted"));
  if (_running) {
    Serial.println(F("\tAlready running."));
    return;
  }

  _airborne = false;
  _burnoutTime = _config.MotorFuelMass / _config.MotorFuelBurnRate; // s
  _elapsedTime = 0.0;
  _maxHeight = 0;
  _maxVelocity = 0;
  _motorThrust = _config.MotorExhaustVelocity * _config.MotorFuelBurnRate; // N
  _startingAltitude = EarthRadius + initialAltitude;
  _trace[0] = _startingAltitude; // Initial position
  _trace[1] = 0.0; // Initial velocity

  Serial.println(F("sim -\tTime\tDelta\tThrust\tMass\tVelocity\tStarting Altitude\tPosition\tHeight\tAirborne"));

  _running = true;
  // Serial.println(F("Simulation\tCreating task..."));
  BaseType_t  xReturned = xTaskCreatePinnedToCore(
    &simulation::simulationTaskW, /* Function to implement the task */
    "simulationTask", /* Name of the task */
    4000,  /* Stack size in words */
    this,  /* Task input parameter */
    0,  /* Priority of the task */
    &simulationTaskHandle,  /* Task handle. */
    0 /* Core where the task should run */
  );
  // if (xReturned == pdPASS) {
  //     /* The task was created. Use the task's handle to delete the task. */
  //     vTaskDelete(simulationTaskHandle);
  // }
}

void simulation::stop() {
  if (_running) {
    Serial.println(F("Simulation\tStopped."));
    Serial.println(F(""));
    _running = false;
    return;
  }
  Serial.println(F("Simulation\tNo simulation was running."));
  Serial.println(F(""));
}

double simulation::valueAltitude() {
  if (_running)
    return 0;
  return _trace[0];
}

simulation _simulation;