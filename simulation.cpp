#include <Arduino.h>
#include <ArduinoJson.h>

#include "debug.h"
#include "fileSystem.h"
#include "simulation.h"
#include "utilities.h"

struct simulationConfig simulationConfigDefault;

simulation::simulation() {
}

bool simulation::isRunning() {
  return _running;
}

void simulation::evaluateTimestep(float deltaT, double drag, double Mr, double Me, double G, double Ft, double *x) {
  // Leap-frog Euler method -- interleave calculation of DeltaX, DeltaV
  // Move the position
  _trace[0] += deltaT * _trace[1];
  // Compute the total force
  double F = -G * Me * Mr / (_trace[0] * _trace[0]);
  double dragSign = (_trace[1] < 0) ? 1.0 : -1.0;
  F += 0.5 * drag * (_trace[1] * _trace[1]) * dragSign;
  F += Ft;
  // Apply acceleration
  // _trace[1] += deltaT * F / Mr;
  _trace[2] = F / Mr;
  _trace[1] += deltaT * _trace[2];
}

void simulation::loopStep(float deltaT, bool output) {
  deltaT = deltaT / 1000.0; // to seconds?

  if (_airborne && _trace[0] <= _altitudeStarting) {
#ifdef DEBUG_SIM
    outputPrint(deltaT, 0, 0, 0, 0, 0);
    Serial.println(F("sim -\tLANDED"));
    Serial.print(F("sim -\tMax. Height="));
    Serial.println(_maxAltitude);
    Serial.print(F("sim -\tMax. Velocity="));
    Serial.println(_maxVelocity);
    Serial.println();
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

  double altitude = _trace[0] - _altitudeStarting;
  double airDensity = AirDensity * pow(0.5, altitude / AirDensityScale);
  double drag = airDensity * _config.CrossSection * _config.DragCoefficient;

  double mass = _config.RocketMass;
  double thrust = 0;
  bool burnOut = (_elapsedTime < _burnoutTime);
  if (burnOut) {
    mass = _config.RocketMass + _config.MotorFuelMass - _config.MotorFuelBurnRate * _elapsedTime;
    thrust = _motorThrust; // N
  }

  evaluateTimestep(deltaT, drag, mass, EarthMass, GravConstant, thrust, _trace);
  // TODO: Introduce some  point - recommendation seems to be to use Simplex noise from FastLED

  if (output)
    outputPrint(deltaT, mass, thrust, altitude, airDensity, drag, burnOut);

  if (_trace[0] > _altitudeStarting)
    _airborne = true;

  if (altitude > _maxAltitude)
    _maxAltitude = altitude;
  if (_trace[1] > _maxVelocity)
    _maxVelocity = _trace[1];

  _elapsedTime += deltaT;
}

void simulation::outputPrint(float delta, double mass, double thrust, double altitude, double airDensity, double drag, bool burnOut) {
  char temp[20];
  char formatFloat[5] = "%.4f";
  Serial.print(F("sim -  "));

  sprintf(temp, formatFloat, _elapsedTime);
  Serial.print(temp);
  Serial.print(stringPad(temp, 12));

  sprintf(temp, formatFloat, delta);
  Serial.print(temp);
  Serial.print(stringPad(temp, 12));

  sprintf(temp, formatFloat, _config.RocketMass);
  Serial.print(temp);
  Serial.print(stringPad(temp, 14));

  sprintf(temp, formatFloat, _config.RocketMass + _config.MotorFuelMass);
  Serial.print(temp);
  Serial.print(stringPad(temp, 14));

  sprintf(temp, formatFloat, mass);
  Serial.print(temp);
  Serial.print(stringPad(temp, 15));

  sprintf(temp, formatFloat, thrust);
  Serial.print(temp);
  Serial.print(stringPad(temp, 13));

  sprintf(temp, formatFloat, _trace[2]); // Acceleration
  Serial.print(temp);
  Serial.print(stringPad(temp, 15));

  sprintf(temp, formatFloat, _trace[1]); // Velocity
  Serial.print(temp);
  Serial.print(stringPad(temp, 14));

  sprintf(temp, formatFloat, altitude);
  Serial.print(temp);
  Serial.print(stringPad(temp, 14));

  sprintf(temp, formatFloat, _trace[0]); // Position
  Serial.print(temp);
  Serial.print(stringPad(temp, 18));

  sprintf(temp, formatFloat, airDensity);
  Serial.print(temp);
  Serial.print(stringPad(temp, 13));

  sprintf(temp, formatFloat, drag);
  Serial.print(temp);
  Serial.print(stringPad(temp, 12));

  Serial.print(burnOut ? "true" : "false");
  Serial.print(stringPad("true", 9));

  sprintf(temp, formatFloat, _burnoutTime);
  Serial.print(temp);
  Serial.print(stringPad(temp, 15));

  Serial.println(_airborne ? "true" : "false");
}

void simulation::outputPrintHeader() {
// #ifdef DEBUG_SIM
  // Serial.println(F("sim -\tTime\tDelta\tThrust\tMass\tAcceleration\tVelocity\tPosition\tStarting Altitude\tAltitude\tAirborne"));
  Serial.println(F("sim -  Time       Delta      Rocket Mass  Total Mass   Current Mass  Thrust      Acceleration  Velocity     Altitude     Position         AirDensity  Drag       Burnout  Burnout Time  Airborne"));
// #endif
}

void simulation::outputSerialList() {
  Serial.println(F("\nOutput serial list of sim configs."));

  JsonDocument doc;
  deserializeJson(doc, "[]");
  JsonArray configs = doc.to<JsonArray>();
  _fileSystem.instance.loadConfigSim(configs);
// #ifdef DEBUG_SIM
  // serializeJson(configs, Serial);
// #endif
  if (!configs || configs.size() <= 0) {
    Serial.println(F("\tFailed to load configuration."));
    return;
  }
#ifdef DEBUG_SIM
  Serial.println(F("\nOutput serial list of sim configs loaded."));
  serializeJson(configs, Serial);
  Serial.println();
#endif

  char temp[20];
  char formatFloat[5] = "%.4f";
  Serial.println(F("Number Name                  RocketMassOz  MotorFuelBurnTimeS  MotorFuelMassEmptyOz  MotorFuelMassLaunchOz  MotorThrustN  DiameterMM  DragCoefficient"));
  for (JsonObject obj : configs) {
    int tempN = obj["number"];
    Serial.print(tempN);
    Serial.print(stringPad(tempN, 9));

    String name = obj["name"];
    Serial.print(name.c_str());
    Serial.print(stringPad(name.c_str(), 23));

    float value = obj["RocketMassOz"];
    sprintf(temp, formatFloat, value);
    Serial.print(temp);
    Serial.print(stringPad(temp, 15));

    value = obj["MotorFuelBurnTimeS"];
    sprintf(temp, formatFloat, value);
    Serial.print(temp);
    Serial.print(stringPad(temp, 21));

    value = obj["MotorFuelMassEmptyOz"];
    sprintf(temp, formatFloat, value);
    Serial.print(temp);
    Serial.print(stringPad(temp, 23));

    value = obj["MotorFuelMassLaunchOz"];
    sprintf(temp, formatFloat, value);
    Serial.print(temp);
    Serial.print(stringPad(temp, 24));

    value = obj["MotorThrustN"];
    sprintf(temp, formatFloat, value);
    Serial.print(temp);
    Serial.print(stringPad(temp, 15));

    value = obj["DiameterMM"];
    sprintf(temp, formatFloat, value);
    Serial.print(temp);
    Serial.print(stringPad(temp, 13));

    value = obj["DragCoefficient"];
    sprintf(temp, formatFloat, value);
    Serial.println(temp);
  }

  Serial.println(F("...completed"));
}

void simulation::simulationTaskW(void * parameter) {
    simulation* instance = reinterpret_cast<simulation*>(parameter) ; // obtain the instance pointer
    instance->simulationTask() ; // dispatch to the member function, now that we have an instance pointer
}

void simulation::simulationTask() {
  Serial.println(F("Simulation task..."));
  Serial.print(F("sim -  Sample Rate="));
  Serial.println(_config.sampleRate);

  unsigned long start = _simulationTimestamp = millis();
  int count = 0;
  int countHeader = 0;
  int settle = 0;
  while(_running) {
    bool output = false;
    bool outputHeader = false;

    delayMicroseconds(100); 
    unsigned long currentTimestamp = millis();
    // debug(F("Simulation currentTimestamp..."), currentTimestamp);
    // debug(F("Simulation _simulationTimestamp...", _simulationTimestamp);
    int deltaElapsed = currentTimestamp - _simulationTimestamp;
    // debug(F("Simulation deltaElapsed..."), deltaElapsed);

    int delta = _simulationThrottle.determine(deltaElapsed, _config.sampleRate);
    // debug(F("Simulation delta..."), delta);
    if (delta != 0) {
      // let the loop settle before starting to simulate...
      if (settle < 15) {
        debug(F("Simulation settle..."), settle);
        settle++;
        continue;
      }

      //if (count % 25 == 0) {
      if (count % 5 == 0) {
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

    _simulationTimestamp = currentTimestamp;
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

void simulation::start(long altitudeInitial) {
  Serial.println();
  Serial.println(F("Simulation Started"));

  if (_running) {
    Serial.println(F("\tAlready running."));
    return;
  }
  
  JsonDocument doc;
  JsonArray configs = doc.to<JsonArray>();
  _fileSystem.instance.loadConfigSim(configs);
#ifdef DEBUG_SIM
  serializeJson(configs, Serial);
  Serial.println();
#endif
  if (!configs || configs.size() <= 0) {
    Serial.println(F("\n\tFailed to load configuration."));
    return;
  }
  Serial.println(F("\tSuccessfully loaded configurations."));

  int requestedNumber = 1;
  JsonObject config;
  for (JsonObject obj : configs) {
    int temp = obj["number"];
    if (temp == requestedNumber) {
      config = obj;
      break;
    }
  }
  if (!config) {
    Serial.print(F("\tFailed to find config for requested simulation '"));
    Serial.print(requestedNumber);
    Serial.println(F("'."));
    return;
  }
#ifdef DEBUG_SIM
  serializeJson(config, Serial);
  Serial.println();
#endif

  struct simulationConfig startConfig;
  startConfig.RocketMass = ((float)config["RocketMassOz"]) / 35.274; // kg
  startConfig.MotorFuelBurnTime = config["MotorFuelBurnTimeS"]; // s
  float motorFuelMassEmptyOz = config["MotorFuelMassEmptyOz"];
  float motorFuelMassEmptyKg = motorFuelMassEmptyOz / 35.274; // kg
  float motorFuelMassLaunchOz = config["MotorFuelMassLaunchOz"];
  float motorFuelMassLaunchKg = motorFuelMassLaunchOz / 35.274; // kg
  startConfig.MotorFuelMass = motorFuelMassLaunchKg - motorFuelMassEmptyKg; // kg
  startConfig.MotorFuelBurnRate = startConfig.MotorFuelMass / startConfig.MotorFuelBurnTime;
  startConfig.MotorThrust = config["MotorThrustN"]; // N
  startConfig.CrossSection = pow((((float)config["DiameterMM"]) / 1000), 20.0762); // m^20.0762;
  startConfig.DragCoefficient = config["DragCoefficient"];

  _config = startConfig;
  
  Serial.println();
  Serial.print("RocketMass: ");
  Serial.println(startConfig.RocketMass, 5);
  Serial.print("MotorFuelBurnTime: ");
  Serial.println(startConfig.MotorFuelBurnTime, 5);
  Serial.print("MotorFuelMassEmptyOz: ");
  Serial.println(motorFuelMassEmptyOz, 5);
  Serial.print("MotorFuelMassEmptyKg: ");
  Serial.println(motorFuelMassEmptyKg, 5);
  Serial.print("MotorFuelMassLaunchOz: ");
  Serial.println(motorFuelMassLaunchOz, 5);
  Serial.print("MotorFuelMassLaunchKg: ");
  Serial.println(motorFuelMassLaunchKg, 5);
  Serial.print("MotorFuelMass: ");
  Serial.println(startConfig.MotorFuelMass, 5);
  Serial.print("MotorFuelBurnRate: ");
  Serial.println(startConfig.MotorFuelBurnRate, 5);
  Serial.print("MotorThrust: ");
  Serial.println(startConfig.MotorThrust, 5);
  Serial.print("CrossSection: ");
  Serial.println(startConfig.CrossSection, 5);
  Serial.print("DragCoefficient: ");
  Serial.println(startConfig.DragCoefficient, 5);
  Serial.println();

  _airborne = false;
  _burnoutTime = _config.MotorFuelBurnTime;
  _elapsedTime = 0.0;
  _finalCountdown = 0;
  _maxAltitude = 0;
  _maxVelocity = 0;
  _motorThrust = _config.MotorThrust; // N
  _altitudeInitial = altitudeInitial;
  _altitudeStarting = EarthRadius + altitudeInitial;
  _trace[0] = _altitudeStarting; // Initial position
  _trace[1] = 0.0; // Initial velocity
  _trace[2] = 0.0; // Initial acceleration

  // Serial.println(F("sim -\tTime\tDelta\tThrust\tMass\tVelocity\tStarting Altitude\tPosition\tAltitude\tAirborne"));

  _running = true;
  // Serial.println(F("Simulation\tCreating task..."));
  BaseType_t xReturned = xTaskCreatePinnedToCore(
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
    Serial.println();
    _running = false;
    return;
  }
  Serial.println(F("Simulation\tNo simulation was running."));
  Serial.println();
}

double simulation::valueAltitude() {
  if (!_running)
    return 0;

  double altitude = _trace[0] - _altitudeStarting + _altitudeInitial;
// #ifdef DEBUG_SIM
//   debug("simulation.altitude", altitude);
// #endif
  return altitude;
}

accelerometerValues simulation::valueAcceleration() {
  accelerometerValues values;
  if (!_running)
    return values;

  values.x = 0;
  values.y = 0;
  values.z = _trace[2];

  return values;
}

simulation _simulation;