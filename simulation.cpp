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
  double dragSign = (_trace[1] < 0) ? 1.0 : -1.0;
  F += 0.5 * K * (_trace[1] * _trace[1]) * dragSign;
  F += Ft;
  // Apply acceleration
  _trace[1] += deltaT * F / Mr;
}

void simulation::outputPrint(double delta, double thrust, double mass, double altitude) {
#ifdef DEBUG_SIM
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
  Serial.print(altitude);
  Serial.print(F("\t\t"));
  Serial.println(_airborne ? "true" : "false");
#endif
}

void simulation::outputPrintHeader() {
#ifdef DEBUG_SIM
  Serial.println(F("sim -\tTime\tDelta\tThrust\tMass\tVelocity\tStarting Altitude\tPosition\tAltitude\tAirborne"));
#endif
}

void simulation::loopStep(double deltaT, bool output) {
  deltaT = deltaT / 1000;

  if (_airborne && _trace[0] <= _startingAltitude) {
#ifdef DEBUG_SIM
    outputPrint(deltaT, 0, 0, 0);
    Serial.println(F("sim -\tLANDED"));
    Serial.print(F("sim -\tMax. Height="));
    Serial.println(_maxHeight);
    Serial.print(F("sim -\tMax. Velocity="));
    Serial.println(_maxVelocity);
    Serial.println(F(""));
    Serial.println(_maxVelocity);
    Serial.print(F("final countdown..."));
    Serial.println(_finalCountdown);
#endif

    if (_finalCountdown >= finalCountdown) {
      _finalCountdown++;
      _running = false;
      return;
    }

    _finalCountdown++;
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

  if (output)
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
  Serial.println(F("Simulation task..."));
  Serial.print(F("sim -\tSample Rate="));
  Serial.println(_config.sampleRate);

  unsigned long start = millis();
  int count = 0;
  int countHeader = 0;
  int settle = 0;
  while(_running) {
    // debug(F("Simulation _simulationTimestamp...", _simulationTimestamp);
    unsigned long current = millis();
    // debug(F("Simulation current..."), current);

    // unsigned long deltaT = current - _simulationTimestamp;
    // debug(F("Simulation deltaT..."), deltaT);

    bool output = false;
    bool outputHeader = false;

    int deltaElapsed = current - _simulationTimestamp;
    int delta = _simulationThrottle.determine(deltaElapsed, _config.sampleRate);
    // debug(F("Simulation delta..."), delta);
    if (delta != 0) {
      // debug(F("Simulation settle..."), settle);
      // let the loop settle before starting to simulate...
      if (settle < 5) {
        settle++;
        continue;
      }

      if (count % 25 == 0) {
        count = 0;
        output = true;
        if (countHeader % 10 == 0) {
          countHeader = 0;
          countHeader++;
          outputHeader = true;
        }
        countHeader++;
      }
      count++;

      if (outputHeader)
        outputPrintHeader();

      // debug(F("Simulation, delta"), delta);
      loopStep(delta, output);
    }

    _simulationTimestamp = current;
  }
  
  unsigned long stop = millis();
  unsigned long runtime = stop - start;
  Serial.print(F("sim -\tStart="));
  Serial.println(start);
  Serial.print(F("sim -\tStop="));
  Serial.println(stop);
  Serial.print(F("sim -\tRuntime (ms)="));
  Serial.println(runtime);
  Serial.print(F("sim -\tRuntime (s)="));
  Serial.println(runtime / 1000);

  // Delete the task...
  vTaskDelete(NULL);
}

void simulation::start(simulationConfig startConfig, long initialAltitude) {
  _config = startConfig;
  Serial.println(F(""));
  Serial.println(F("Simulation Started"));
  if (_running) {
    Serial.println(F("\tAlready running."));
    return;
  }

  _airborne = false;
  _burnoutTime = _config.MotorFuelMass / _config.MotorFuelBurnRate; // s
  _elapsedTime = 0.0;
  _finalCountdown = 0;
  _maxHeight = 0;
  _maxVelocity = 0;
  _motorThrust = _config.MotorExhaustVelocity * _config.MotorFuelBurnRate; // N
  _startingAltitude = EarthRadius + initialAltitude;
  _trace[0] = _startingAltitude; // Initial position
  _trace[1] = 0.0; // Initial velocity

  // Serial.println(F("sim -\tTime\tDelta\tThrust\tMass\tVelocity\tStarting Altitude\tPosition\tAltitude\tAirborne"));

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
  if (!_running)
    return 0;

  double altitude = _trace[0] - _startingAltitude;
  debug("simulation.altitude", altitude);
  return altitude;
}

simulation _simulation;