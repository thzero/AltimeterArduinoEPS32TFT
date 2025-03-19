#include <Arduino.h>
#include <ArduinoJson.h>

#include "constants.h"
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
  // TODO: Introduce some  point - recommendation seems to be to use Simplex noise from FastLED
  // Leap-frog Euler method -- interleave calculation of DeltaX, DeltaV
  // Compute the total force
  double F = -G * Me * Mr / pow(_trace[0], 2);
  double dragSign = (_trace[1] < 0) ? 1.0 : -1.0;
  // Fd = .5 p V^2 Cd A
  // A = 1/4 pi * D^2
  F += 0.5 * pow(_trace[1], 2) * drag * dragSign;
  F += Ft;
  // Apply acceleration
  // _trace[1] += deltaT * F / Mr;
  _trace[2] = F / Mr;
  _trace[1] += deltaT * _trace[2];
  // Move the position
  _trace[0] += deltaT * _trace[1];
}

void simulation::loopStep(float deltaT, bool output, bool outputHeader) {
  deltaT = deltaT / 1000.0; // to seconds?

  // Altitude is the flight altitude minus starting altitude.
  double altitudeFlight = _trace[0] - _altitudeStarting;

  double airDensity = AirDensity * pow(0.5, altitudeFlight / AirDensityScale);
  // Fd = .5 p V^2 Cd A
  // p = air density
  // Cd = drag coefficient
  // A = Frontal Area
  double drag = airDensity * _config.FrontalArea * _config.DragCoefficient;

  double mass = _config.RocketMass;
  double thrust = 0;
  bool burnout = (_elapsedTime >= _burnoutTime);
  if (!burnout) {
    mass = _config.RocketMass + _config.MotorFuelMass - _config.MotorFuelBurnRate * _elapsedTime;
    thrust = _motorThrust; // N
  }

  // if airborne, its passed altitude, and the velocity is equal to zero or less than zero...
  if (_airborne && _airborneApogee && (altitudeFlight <= 0)) {
    // then we've landeded...
     outputPrint(deltaT, mass, thrust, altitudeFlight, airDensity, drag, burnout, 0);
#ifdef DEBUG_SIM
    Serial.println(F("sim - LANDED"));
    Serial.print(F("sim -  Max. Height: "));
    Serial.println(_maxAltitude);
    Serial.print(F("sim -  Max. Velocity: "));
    Serial.println(_maxVelocity);
    Serial.print(F("sim -  Settle Final: "));
    Serial.print(_settleFinal);
    Serial.print(F(" < "));
    Serial.println(SettleFinal);
#endif

    if (_settleFinal < SettleFinal) {
      _settleFinal++;
      return;
    }

    stop();
    return;
  }

  evaluateTimestep(deltaT, drag, mass, EarthMass, GravConstant, thrust, _trace);

  if (_trace[0] > _altitudeStarting)
    _airborne = true;

  double altitudeFlightDelta = 0;
  if (!_airborneApogee) {
    altitudeFlightDelta = altitudeFlight - _altitudeFlightLast;
    // Detect apogee by taking X number of measures as long as the current is less
    // than the last recorded altitude.
#ifdef DEBUG_SIM
    Serial.print(F("loopStep...altitudeFlight: "));
    Serial.print(altitudeFlight);
    Serial.print(F(", altitudeLast: "));
    Serial.print(_altitudeFlightLast);
    Serial.print(F(", altitudeFlightDelta: "));
    Serial.println(altitudeFlightDelta);
#endif
    if (altitudeFlightDelta < 0) {
      _altitudeApogeeMeasures = _altitudeApogeeMeasures - 1;
      if (_altitudeApogeeMeasures == 0) {
        // Detected apogee.
        _altitudeApogee = altitudeFlight;
        _airborneApogee = true;
        return;
      }
    } 
    else {
      if (_altitudeApogeeMeasures < SAMPLE_MEASURES_APOGEE) {
#ifdef DEBUG_SIM
        Serial.println(F("loopStep...measures - reset"));
#endif
        // If the curent is greater than the last, then reset as it was potentially a false positive.
        _altitudeFlightLast = altitudeFlight;
        _altitudeApogeeMeasures = SAMPLE_MEASURES_APOGEE;
      }
    }
  }

  // if (output) {
    if (outputHeader)
        outputPrintHeader();
    outputPrint(deltaT, mass, thrust, altitudeFlight, airDensity, drag, burnout, altitudeFlightDelta);
  // }

  _altitudeFlightLast = altitudeFlight;
  if (altitudeFlight > _maxAltitude)
    _maxAltitude = altitudeFlight;
  if (_trace[1] > _maxVelocity)
    _maxVelocity = _trace[1];

  _elapsedTime += deltaT;
}

void simulation::outputPrint(float delta, double mass, double thrust, double altitude, double airDensity, double drag, bool burnout, double apogeeDelta) {
  char temp[20];
  char formatFloat[5] = "%.4f";
  Serial.print(F("sim -  "));

  sprintf(temp, formatFloat, _elapsedTime);
  Serial.print(temp);
  Serial.print(stringPad(temp, 12));

  sprintf(temp, formatFloat, delta);
  Serial.print(temp);
  Serial.print(stringPad(temp, 12));

  const char* resultsA = _airborne && _airborneApogee ? "Descent" : _airborne ? "Ascent" : "Ground";
  Serial.print(resultsA);
  Serial.print(stringPad(resultsA, 11));

  sprintf(temp, formatFloat, _config.RocketMass);
  Serial.print(temp);
  Serial.print(stringPad(temp, 11));

  sprintf(temp, formatFloat, _config.RocketMass + _config.MotorFuelMass);
  Serial.print(temp);
  Serial.print(stringPad(temp, 10));

  sprintf(temp, formatFloat, mass);
  Serial.print(temp);
  Serial.print(stringPad(temp, 12));

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

  sprintf(temp, formatFloat, _trace[0] - EarthRadius); // Position
  Serial.print(temp);
  Serial.print(stringPad(temp, 14));

  sprintf(temp, formatFloat, _trace[0]); // Position
  Serial.print(temp);
  Serial.print(stringPad(temp, 18));

  sprintf(temp, formatFloat, airDensity);
  Serial.print(temp);
  Serial.print(stringPad(temp, 14));

  sprintf(temp, formatFloat, _config.FrontalArea);
  Serial.print(temp);
  Serial.print(stringPad(temp, 16));

  sprintf(temp, formatFloat, _config.DragCoefficient);
  Serial.print(temp);
  Serial.print(stringPad(temp, 13));

  sprintf(temp, formatFloat, drag);
  Serial.print(temp);
  Serial.print(stringPad(temp, 10));

  const char* resultsB = burnout ? "true" : "false";
  Serial.print(resultsB);
  Serial.print(stringPad(resultsB, 11));

  sprintf(temp, formatFloat, _burnoutTime);
  Serial.print(temp);
  Serial.print(stringPad(temp, 10));

  sprintf(temp, "%d", _altitudeApogeeMeasures);
  Serial.print(temp);
  Serial.print(stringPad(temp, 11));

  sprintf(temp, formatFloat, apogeeDelta);
  Serial.print(temp);
  Serial.print(stringPad(temp, 14));

  Serial.println(_airborneApogee ? "true" : "false");
}

void simulation::outputPrintHeader() {
  Serial.println(F("sim -                                             Mass                                                              Altitude                                                                               Burnout                 Apogee"));
  Serial.println(F("sim -                                  ----------------------------                                         ------------------------                                                                    ----------------  -----------------------------"));
  Serial.println(F("sim -  Time       Delta      Status    Rocket    Total    Current    Thrust      Acceleration  Velocity     Flight       Initial      Position         Air Density  Cross Section  Drag Coeff  Drag     Burnout  Time     Measures  Delta        Apogee"));
}

void simulation::outputSerialList() {
  Serial.println(F("\nOutput serial list of sim configs."));

  JsonDocument doc;
  deserializeJson(doc, "[]");
  JsonArray configs = doc.to<JsonArray>();
  _fileSystem.instance.loadConfigSim(configs);
#ifdef DEBUG_SIM
  serializeJson(configs, Serial);
#endif
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
  _running = true;
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
      if (settle < SettleStart) {
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

      // debug(F("Simulation, delta"), delta);
      loopStep(delta, output, outputHeader);
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

void simulation::start(int requestedNumber, long altitudeInitial, long altitudeFinal) {
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

  JsonObject config;
  for (JsonObject obj : configs) {
    int temp = obj["number"];
    if (temp == requestedNumber) {
      config = obj;
      break;
    }
  }
  if (!config) {
    Serial.print(F("\tFailed to find config for requested simulation #"));
    Serial.print(requestedNumber);
    Serial.println(F("."));
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
  float diameterMM = config["DiameterMM"];
  diameterMM = diameterMM / 1000;
  // A = 1/4 pi * D^2
  startConfig.FrontalArea = 0.25 * M_PI * pow(diameterMM, 2);
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
  Serial.print("DiameterMM: ");
  Serial.println(diameterMM, 5);
  Serial.print("FrontalArea: ");
  Serial.println(startConfig.FrontalArea, 5);
  Serial.print("DragCoefficient: ");
  Serial.println(startConfig.DragCoefficient, 5);
  Serial.println();

  _airborne = false;
  _airborneApogee = false;
  _burnoutTime = _config.MotorFuelBurnTime;
  _elapsedTime = 0.0;
  _maxAltitude = 0;
  _maxVelocity = 0;
  _motorThrust = _config.MotorThrust; // N
  _altitudeApogee = -1;
  _altitudeApogeeMeasures = SAMPLE_MEASURES_APOGEE;
  _altitudeEnding = altitudeFinal;
  _altitudeInitial = altitudeInitial;
  _altitudeFlightLast = 0;
  _altitudeStarting = EarthRadius + altitudeInitial;
  _settleFinal = 0;
  _trace[0] = _altitudeStarting; // Initial position
  _trace[1] = 0.0; // Initial velocity
  _trace[2] = 0.0; // Initial acceleration

  // Serial.println(F("sim -\tTime\tDelta\tThrust\tMass\tVelocity\tStarting Altitude\tPosition\tAltitude\tAirborne"));

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

  // Trace has the Earth Radius and Starting Altitude built into it.
  // Sensors only account for AGL so removing the EarthRadius.
  double altitude = _trace[0] - EarthRadius;
#ifdef DEBUG_SIM
  debug("simulation.altitude", altitude);
#endif
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