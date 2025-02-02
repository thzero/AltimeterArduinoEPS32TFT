#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Button2.h>
#include <esp_sleep.h>
#include <Preferences.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>
#include <Wire.h>

// #include "ble.h"
#include "button.h"
#include "commands.h"
#include "constants.h"
#include "debug.h"
#include "flightLogger.h"
#include "flightLoggerBase.h"
#include "flightLoggerData.h"
#include "flightLoggerLFS.h"
#include "kalman.h"
#include "leds.h"
#include "loopThrottle.h"
#include "neoPixel.h"
#include "sensor.h"
#include "utilities.h"
#include "wifi.h"
#include "images/thzero_altimeters128x128.h"

// Assumed environmental values
float altitudeBarometer = 1650.3;  // meters ... map readings + barometer position
byte altitudeOffsetLiftoff = 20;
int altitudeOffsetGround = altitudeOffsetLiftoff / 2;
int timeoutRecording = 300 * 1000;
int timeOutTimeToApogee = 2000;

//////////////////////////////////////////////////////////////////////
// Global variables
//////////////////////////////////////////////////////////////////////
TFT_eSPI _tft = TFT_eSPI();

GraphWidget _graphWidget = GraphWidget(&_tft);  // Graph widget
TraceWidget _graphTraceAltitude = TraceWidget(&_graphWidget);
TraceWidget _graphTraceTemperature = TraceWidget(&_graphWidget);
TraceWidget _graphTracePressure = TraceWidget(&_graphWidget);
TraceWidget _graphTraceAccelX = TraceWidget(&_graphWidget);
TraceWidget _graphTraceAccelY = TraceWidget(&_graphWidget);
TraceWidget _graphTraceAccelZ = TraceWidget(&_graphWidget);
TraceWidget _graphTraceHumidity = TraceWidget(&_graphWidget);

enum loopStates {
  AIRBORNE_ASCENT,
  AIRBORNE_DESCENT,
  GROUND
};

struct loopStateMachine {
  enum loopStates current = GROUND;
} _loopState;

bool _drawSplash = true;
unsigned long _timestamp;

void drawTftFlight(unsigned long timestamp, unsigned long delta) {
  if (_tft.getRotation() == 1) {
    _tft.setRotation(0);
    _tft.fillScreen(TFT_BLACK);
  }

  _tft.setCursor(0, STATUS_HEIGHT_BAR);
  _tft.println("                                     ");

  char altitude[40];
  sprintf(altitude, "Int. Alt. = %.2f m    ", _flightLogger.data.altitudeInitial);
  _tft.println(altitude);

  if (_flightLogger.aborted) {
    _tft.println("ABORTED!!!");
    return;
  }

  sprintf(altitude, "Cur. Alt. = %.2f m    ", _flightLogger.data.altitudeCurrent);
  _tft.println(altitude);

  drawTftSensorImu();
}

void drawTftFlightAirborne(unsigned long timestamp, unsigned long delta) {
  if (_tft.getRotation() == 1) {
    _tft.setRotation(0);
    _tft.fillScreen(TFT_BLACK);
  }

  _tft.setCursor(0, STATUS_HEIGHT_BAR);
  _tft.println("Recording in progress .....");

  char altitude[40];
  sprintf(altitude, "Int. Alt. = %i m    ", _flightLogger.data.altitudeInitial);
  _tft.println(altitude);

  if (_flightLogger.aborted) {
    _tft.println("ABORTED!!!");
    return;
  }
  sprintf(altitude, "Cur. Alt. = %i m    ", _flightLogger.data.altitudeCurrent);
  _tft.println(altitude);

  drawTftSensorImu();
}

void drawTftGraphAxesXY(float minX, float maxX, float minY, float maxY, int flightNbr, char *curveName) {
  _tft.fillScreen(TFT_BLACK);

  // x scale units is from 0 to 100, y scale units is 0 to 50
  _graphWidget.setGraphScale(minX, maxX, minY, maxY);
  // X grid starts at 0 with lines every 10 x-scale units
  // Y grid starts at 0 with lines every 10 y-scale units
  // blue grid
  _graphWidget.setGraphGrid(0.0, maxX / 5, 0.0, maxY / 5, TFT_BLUE);

  // Draw empty graph, top left corner at 20,10 on TFT
  _graphWidget.drawGraph(30, 10);

  // Draw the x axis scale
  _tft.setTextDatum(TC_DATUM);  // Top centre text datum
  _tft.drawNumber(minX, _graphWidget.getPointX(minX), _graphWidget.getPointY(0) + 3);

  if (maxX < 1000) {
    _tft.drawNumber(maxX / 2, _graphWidget.getPointX(maxX / 2), _graphWidget.getPointY(0) + 3);
    _tft.drawNumber(maxX, _graphWidget.getPointX(maxX), _graphWidget.getPointY(0) + 3);
    _tft.drawString("time(ms)", _graphWidget.getPointX(maxX / 4), _graphWidget.getPointY(0) + 3);
  } 
  else {
    char temp[10];
    sprintf(temp, "%3.1f", maxX / 1000 / 2);
    _tft.drawString(temp, _graphWidget.getPointX(maxX / 2), _graphWidget.getPointY(0) + 3);
    sprintf(temp, "%3.1f", maxX / 1000);
    _tft.drawString(temp, _graphWidget.getPointX(maxX) - 10, _graphWidget.getPointY(0) + 3);
    _tft.drawString("time (s)", _graphWidget.getPointX(maxX / 4), _graphWidget.getPointY(0) + 3);
  }

  char flight[15];
  sprintf(flight, "Flight %i  ", flightNbr);
  _tft.drawString(flight, _graphWidget.getPointX(maxX) - 10, _graphWidget.getPointY(maxY));
  _tft.drawString(curveName, _graphWidget.getPointX(maxX / 3), _graphWidget.getPointY(maxY));

  // Draw the y axis scale
  _tft.setTextDatum(MR_DATUM);  // Middle right text datum
  if (maxY < 1000)
    _tft.drawNumber(maxY, _graphWidget.getPointX(0.0), _graphWidget.getPointY(maxY));
  else
    _tft.drawNumber(round2dec(maxY / 1000), _graphWidget.getPointX(0.0), _graphWidget.getPointY(maxY));
}

void drawTftGraphForlightNbr(int flightNbr, int curveType) {
  Serial.println("Display graph on tft...");

  if (_tft.getRotation() == 1) {
    _tft.setRotation(0);
    _tft.fillScreen(TFT_BLACK);
  }

  _tft.drawString("altitude", 6, 0);
  // Graph area is 200 pixels wide, 150 high, dark grey background
  _graphWidget.createGraph(200, 100, _tft.color565(5, 5, 5));
  // x scale units is from 0 to 100, y scale units is 0 to 50
  _graphWidget.setGraphScale(0.0, 100.0, 0, 50.0);

  _tft.setTextColor(TFT_WHITE, TFT_BLACK);
  _tft.fillScreen(TFT_BLACK);
  _tft.setSwapBytes(true);
  _tft.setTextFont(2);

  Serial.println("...display graph on tft successful.");

  if (!_flightLogger.instance.readFlight(flightNbr))
    return;

  flightDataStruct *currentFlight;
  currentFlight = _flightLogger.instance.getFlightData();
  _flightLogger.instance.determineFlightMinAndMax(flightNbr);

  //altitude
  if (curveType == 0) {
    // Start altitude trace
    _graphTraceAltitude.startTrace(TFT_GREEN);
    if ((float)_flightLogger.instance.getFlightAltitudeMax() < 1000)
      drawTftGraphAxesXY(0.0, _flightLogger.instance.getFlightDuration(), 0, (float)_flightLogger.instance.getFlightAltitudeMax(), flightNbr, "Altitude (meters)");
    else
      drawTftGraphAxesXY(0.0, _flightLogger.instance.getFlightDuration(), 0, (float)_flightLogger.instance.getFlightAltitudeMax(), flightNbr, "Altitude (km)");
  }

  //accel
  if (curveType == 1) {
    _graphTraceAccelX.startTrace(TFT_RED);
    _graphTraceAccelY.startTrace(TFT_PURPLE);
    _graphTraceAccelY.startTrace(TFT_YELLOW);

    float maxAccel = 0.0f;
    if (_flightLogger.instance.getFlightAccelXMax() > maxAccel)
      maxAccel = (float)_flightLogger.instance.getFlightAccelXMax();
    if (_flightLogger.instance.getFlightAccelYMax() > maxAccel)
      maxAccel = (float)_flightLogger.instance.getFlightAccelYMax();
    if (_flightLogger.instance.getFlightAccelZMax() > maxAccel)
      maxAccel = (float)_flightLogger.instance.getFlightAccelZMax();

    // Serial.println(maxAccel);
    drawTftGraphAxesXY(0.0, _flightLogger.instance.getFlightDuration(), 0, (float)roundUp(maxAccel), flightNbr, "Accel X,Y,Z (m/s)");
  }

  // pressure
  if (curveType == 2) {
    _graphTracePressure.startTrace(TFT_GREY);
    //Serial.println(_flightLogger.instance.getFlightPressureMax());
    drawTftGraphAxesXY(0.0, _flightLogger.instance.getFlightDuration(), 0, (float)roundUp(_flightLogger.instance.getFlightPressureMax()), flightNbr, "Pressure (mBar)");
  }
  // temperature
  if (curveType == 3) {
    _graphTraceTemperature.startTrace(TFT_BROWN);
    //Serial.println(_flightLogger.instance.getFlightTemperatureMax());
    drawTftGraphAxesXY(0.0, _flightLogger.instance.getFlightDuration(), 0, (float)roundUp(_flightLogger.instance.getFlightTemperatureMax()), flightNbr, "Temp (°C)");
  }
  // humidity
  if (curveType == 4) {
    _graphTraceHumidity.startTrace(TFT_YELLOW);
    Serial.println(_flightLogger.instance.getFlightHumidityMax());
    drawTftGraphAxesXY(0.0, _flightLogger.instance.getFlightDuration(), 0, (float)roundUp(_flightLogger.instance.getFlightHumidityMax() + 1), flightNbr, "Hum %");
  }

  unsigned long currentTime = 0;
  for (long i = 0; i < _flightLogger.instance.getFlightSize(); i++) {
    currentTime = currentTime + currentFlight[i].diffTime;  //logger.getFlightTimeData();

    //altitude
    if (curveType == 0)
      _graphTraceAltitude.addPoint(currentTime, currentFlight[i].altitude);
    if (curveType == 1) {
      _graphTraceAccelX.addPoint(currentTime, (float)currentFlight[i].accelX);
      _graphTraceAccelY.addPoint(currentTime, (float)currentFlight[i].accelY);
      _graphTraceAccelZ.addPoint(currentTime, (float)currentFlight[i].accelZ);
    }
    if (curveType == 2)
      _graphTracePressure.addPoint(currentTime, currentFlight[i].pressure);
    if (curveType == 3)
      _graphTraceTemperature.addPoint(currentTime, currentFlight[i].temperature);
    if (curveType == 4)
      _graphTraceHumidity.addPoint(currentTime, currentFlight[i].humidity);
  }
}

void drawTftSensorImu() {
  // if (!_qmi.instance.getDataReady())
  //   return;

  // if (_qmi.instance.getAccelerometer(_qmi.acc.x, _qmi.acc.y, _qmi.acc.z)) {
  //   char temp[15];
  //   sprintf(temp, "x=%3.2f m/s", (float)_qmi.acc.x);
  //   _tft.println("");
  //   _tft.println(temp);
  //   sprintf(temp, "y=%3.2f m/s", (float)_qmi.acc.y);
  //   _tft.println(temp);
  //   sprintf(temp, "z=%3.2f m/s", (float)_qmi.acc.z);
  //   _tft.println(temp);
  // }
}

String drawTftSplashPad(float value, int width, char * format) {
  char buffer[20];
  // Convert float to string
  sprintf(buffer, format, value); 
  // Serial.print("value: ");
  // Serial.println(value);
  int valueLength = strlen(buffer);
  // Serial.print("valueLength: ");
  // Serial.println(valueLength);
  valueLength = width - valueLength;
  // Serial.print("valueLength2: ");
  // Serial.println(valueLength);
  if (valueLength < 0)
    valueLength = 0;
  if (valueLength > width)
    valueLength = width;
  // Serial.print("valueLength3: ");
  // Serial.println(valueLength);
  char blank[valueLength + 1];
  snprintf(blank, sizeof(blank), "%*s", valueLength, ""); 
  // Serial.print("blank: ");
  // Serial.println(blank);
  return blank;
}

void drawTftSplash() {
  Serial.println("Display splash on tft...");

  _tft.setRotation(0);
  _tft.fillScreen(TFT_BLACK);

  int row = 125;

  _tft.pushImage(6, 0, 128, 128, thzero_altimeters128x128);
  _tft.drawString(BOARD_NAME, 6, row);
  row += 10;

  String version = "ver ";
  version.concat(MAJOR_VERSION);
  version.concat(".");
  version.concat(MINOR_VERSION);
#if defined(DEV) || defined(DEV_SIM)
    version.concat(" (DEV)");
#endif
  _tft.drawString(version, 6, row);
  row += 10;

  _tft.drawString(COPYRIGHT, 6, row);
  row += 10;
  _tft.drawString(COPYRIGHT_YEARS, 6, row);
  row += 15;

  if (_wifi.enabled()) {
    String wifi = "SSID: ";
    wifi.concat(_wifi.ssid());
    _tft.drawString(wifi, 6, row);
    row += 10;
    String ipAddress = "IP: ";
    ipAddress.concat(_wifi.ipAddress());
    _tft.drawString(ipAddress, 6, row);
  }
  else {
    _tft.drawString("Enable WiFi", 6, row);
    row += 10;
    _tft.drawString("5 sec long Press", 6, row);
  }
  row += 15;

  _tft.drawString("Alt.   Temp. Pres.", 6, row);
  row += 10;
  char temp[40];
  sprintf(temp, "%.0fm%s%.0fC%s%.0fhPa", _flightLogger.data.altitudeInitial, drawTftSplashPad(_flightLogger.data.altitudeInitial, 6, "%.0f"), _flightLogger.data.temperatureInitial, drawTftSplashPad(_flightLogger.data.temperatureInitial, 5, "%.0f"), _flightLogger.data.pressureInitial);
  _tft.drawString(temp, 6, row);
  row += 15;
  
  if (_flightLogger.aborted) {
    _tft.drawString("ABORTED!!!", 6, row);
    return;
  }

  Serial.println("...display splash on tft successful.");
}

/*
  This will turn off the altimeter
*/
void sleepDevice() {
  setupButtonDeninit();
  setupNeoPixelBlinkerDeninit();

  _tft.fillScreen(TFT_BLACK);
  _tft.setRotation(0);
  _tft.drawString("turning off...", 6, 185);

  // digitalWrite(4, LOW);
  // delay(2000);

  // neilrbowen...
  pinMode(TFT_BACKLITE, OUTPUT);
  pinMode(TFT_I2C_POWER, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(TFT_BACKLITE, LOW);
  digitalWrite(TFT_I2C_POWER, LOW);
  delay(100);

  gpio_deep_sleep_hold_en();
  gpio_hold_en((gpio_num_t)TFT_BACKLITE);
  gpio_hold_en((gpio_num_t)TFT_I2C_POWER);
  delay(2000);
  // ...neilrbowen

  esp_deep_sleep_start();
}

void loopStateAIRBORNE_ASCENTToAIRBORNE_DESCENT() {
  _loopState.current = AIRBORNE_DESCENT;  // TODO!
}

float loopStateAIRBORNE(unsigned long currentTimestamp, long diffTime) {
  atmosphereValues atmosphere = readSensorAtmosphere();
  float altitude = readSensorAltitude(atmosphere);
  debug("loopStateAIRBORNE...altitude", altitude);
  float altitudeDelta = altitude - _flightLogger.data.altitudeLast;
  debug("loopStateAIRBORNE...altitudeDelta", altitudeDelta);
  _flightLogger.data.altitudeCurrent = altitude;

  // Log the flight altitude...
  _flightLogger.instance.setFlightTime(diffTime);
  _flightLogger.instance.setFlightAltitude(_flightLogger.data.altitudeCurrent);

  // Log the flight temperature and pressure...
  atmosphereValues val = readSensorAtmosphere();
  _flightLogger.instance.setFlightHumidity((long)val.temperature);
  _flightLogger.instance.setFlightTemperature((long)val.temperature);
  _flightLogger.instance.setFlightPressure((long)val.pressure);

  // Log the flight x, y, and z accelerations...
  // if (_qmi.instance.getDataReady()) {
  //   if (_qmi.instance.getAccelerometer(_qmi.acc.x, _qmi.acc.y, _qmi.acc.z)) {
  //     _flightLogger.instance.setFlightAccelX(_qmi.acc.x);
  //     _flightLogger.instance.setFlightAccelY(_qmi.acc.y);
  //     _flightLogger.instance.setFlightAccelZ(_qmi.acc.z);
  //   }
  // }

  // Add to the set data to the current flight.
  _flightLogger.instance.addToCurrentFlight();

  _flightLogger.data.timestampPrevious = currentTimestamp;
  _flightLogger.data.altitudeLast = altitude;

  // sendTelemetry(timestamp - _flightLogger.data.timestampInitial , atmosphere, accelerometer, gryoscope, altitude, diffTime);

  drawTftFlightAirborne(currentTimestamp, diffTime);

  return altitude;
}

loopThrottle _throttleAirborneAscent;
void loopStateAIRBORNE_ASCENT(unsigned long timestamp, unsigned long deltaIn) {
  int delta = _throttleAirborneAscent.determine(timestamp, (int)SAMPLE_RATE_AIRBORNE_ASCENT);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  // atmosphereValues atmosphere = readSensorAtmosphere();
  // float altitude = readSensorAltitude(atmosphere);
  // debug("loopStateAIRBORNE_DESCENT...altitude", altitude);
  // float altitudeDelta = altitude - _flightLogger.data.altitudeLast;
  // debug("loopStateAIRBORNE_DESCENT...altitudeDelta", altitudeDelta);
  // _flightLogger.data.altitudeCurrent = altitude;

  // long currentTimestamp = millis() - _flightLogger.data.timestampLaunch;

  // // Determine different in time between the last step...
  // long diffTime = currentTimestamp - _flightLogger.data.timestampPrevious;

  // // Log the flight altitude...
  // _flightLogger.instance.setFlightTime(diffTime);
  // _flightLogger.instance.setFlightAltitude(_flightLogger.data.altitudeCurrent);

  // // Log the flight temperature and pressure...
  // double temperature, pressure;
  // atmosphereValues val = readSensorAtmosphere();
  // _flightLogger.instance.setFlightHumidity((long)val.temperature);
  // _flightLogger.instance.setFlightTemperature((long)val.temperature);
  // _flightLogger.instance.setFlightPressure((long)val.pressure);

  // // Log the flight x, y, and z accelerations...
  // // if (_qmi.instance.getDataReady()) {
  // //   if (_qmi.instance.getAccelerometer(_qmi.acc.x, _qmi.acc.y, _qmi.acc.z)) {
  // //     _flightLogger.instance.setFlightAccelX(_qmi.acc.x);
  // //     _flightLogger.instance.setFlightAccelY(_qmi.acc.y);
  // //     _flightLogger.instance.setFlightAccelZ(_qmi.acc.z);
  // //   }
  // // }

  // // Add to the set data to the current flight.
  // _flightLogger.instance.addToCurrentFlight();

  // _flightLogger.data.timestampPrevious = currentTimestamp;
  // _flightLogger.data.altitudeLast = altitude;

  // // sendTelemetry(timestamp - _flightLogger.data.timestampInitial , atmosphere, accelerometer, gryoscope, altitude, deltaIn);

  // // Draw the output...
  // drawTftFlightAirborne(timestamp, deltaIn);

  long currentTimestamp = millis() - _flightLogger.data.timestampLaunch;

  // Determine different in time between the last step...
  long diffTime = currentTimestamp - _flightLogger.data.timestampPrevious;

  float altitude = loopStateAIRBORNE(currentTimestamp, diffTime);

  // Detect apogee by taking X number of measures as long as the current is less
  // than the last recorded altitude.
  if (_flightLogger.data.altitudeCurrent < _flightLogger.data.altitudeLast) {
    _flightLogger.data.measures = _flightLogger.data.measures - 1;
    if (_flightLogger.data.measures == 0) {
      // Detected apogee.
      _flightLogger.data.altitudeApogee = _flightLogger.data.altitudeLast;
      _flightLogger.data.timestampApogee = currentTimestamp;
      // DESCENT
      // _loopState.current = AIRBORNE_DESCENT;
      debug("DESCENT!!!!");
      loopStateAIRBORNE_ASCENTToAIRBORNE_DESCENT();
      return;
    }
  } 
  else {
    // If the curent is greater than the last, then reset as it was potentially
    // a false positive.
    _flightLogger.data.altitudeLast = _flightLogger.data.altitudeCurrent;
    _flightLogger.data.measures = 5;
  }

  // Check for timeouts...
  bool timestampApogeeCheck = (millis() - _flightLogger.data.timestampApogee) > timeOutTimeToApogee;
  debug("loopStateAIRBORNE_ASCENT...timestampApogeeCheck", timestampApogeeCheck);
  bool timeoutRecordingCheck = (millis() - _flightLogger.data.timestampLaunch) > timeoutRecording;
  debug("loopStateAIRBORNE_ASCENT...timeoutRecordingCheck", timeoutRecordingCheck);

  if (timestampApogeeCheck) {
    // Something went wrong and apogee was never found, so abort!
    loopStateAIRBORNEToAbort("Time to apogee threshold exceeded!", "AIRBORNE_ASCENT aborted, returning to GROUND!!!!");
    return;
  }

  if (timeoutRecordingCheck) {
    // Something went wrong., so abort!
    loopStateAIRBORNEToAbort("Time to apogee threshold exceeded!", "AIRBORNE_ASCENT aborted, returning to GROUND!!!!");
    return;
  }
}

void loopStateAIRBORNEToAbort(char message1[], char message2[]) {
  // Something went wrong and aborting...
  Serial.println(message1);
  Serial.println(message2);

  // _loopState.current = GROUND; // TODO
  _flightLogger.data.airborne = false;
  _flightLogger.aborted = true;
  _flightLogger.recording = false;
  _flightLogger.data.touchdown = true;

  // To avoid battery drain, etc. user should choose to turn on networking...
  // setupNetwork();

  drawTftSplash();
}

void loopStateAIRBORNEToGROUND() {
  // Complete the flight
  Serial.println("Flight has ended!!!");

  // _loopState.current = GROUND; // TODO
  _flightLogger.data.airborne = false;
  _flightLogger.recording = false;

  // To avoid battery drain, etc. user should choose to turn on networking...
  // setupNetwork();

  drawTftSplash();
}

loopThrottle _throttleAirborneDescent;
void loopStateAIRBORNE_DESCENT(unsigned long timestamp, unsigned long deltaIn) {
  int delta = _throttleAirborneDescent.determine(timestamp, (int)SAMPLE_RATE_AIRBORNE_DESCENT);
  if (delta == 0)
    return;

  // Functionality that happen on the tick goes here:

  // atmosphereValues atmosphere = readSensorAtmosphere();
  // float altitude = readSensorAltitude(atmosphere);
  // debug("loopStateAIRBORNE_DESCENT...altitude", altitude);
  // float altitudeDelta = altitude - _flightLogger.data.altitudeLast;
  // debug("loopStateAIRBORNE_DESCENT...altitudeDelta", altitudeDelta);
  // _flightLogger.data.altitudeCurrent = altitude;

  // // Log the flight x, y, and z accelerations...
  // // if (_qmi.instance.getDataReady()) {
  // //   if (_qmi.instance.getAccelerometer(_qmi.acc.x, _qmi.acc.y, _qmi.acc.z)) {
  // //     _flightLogger.instance.setFlightAccelX(_qmi.acc.x);
  // //     _flightLogger.instance.setFlightAccelY(_qmi.acc.y);
  // //     _flightLogger.instance.setFlightAccelZ(_qmi.acc.z);
  // //   }
  // // }

  // // sendTelemetry(timestamp - _flightLogger.data.timestampInitial , atmosphere, accelerometer, gryoscope, altitude, deltaIn);

  // drawTftFlightAirborne(timestamp, deltaIn);

  long currentTimestamp = millis() - _flightLogger.data.timestampLaunch;

  // Determine different in time between the last step...
  long diffTime = currentTimestamp - _flightLogger.data.timestampPrevious;

  float altitude = loopStateAIRBORNE(currentTimestamp, diffTime);

  debug("loopStateAIRBORNE_DESCENT...altitudeOffsetGround", altitudeOffsetGround);
  bool altitudeCheck = _flightLogger.data.altitudeCurrent < altitudeOffsetGround;
  debug("loopStateAIRBORNE_DESCENT...altitudeCheck", altitudeCheck);
  bool timeoutRecordingCheck = ((timestamp - _flightLogger.data.timestampLaunch) > timeoutRecording);
  debug("loopStateAIRBORNE_DESCENT...timeoutRecordingCheck", timeoutRecordingCheck);

  if (timeoutRecordingCheck) {
    // Something went wrong and the recordingt timeout was hit, so abort!
    loopStateAIRBORNEToAbort("Exceeded recording timeout!", "AIRBORNE_DESCENT aborted, returning to GROUND!!!!");
    return;
  }

  if (altitudeCheck) {
    _flightLogger.data.altitudeTouchdown = _flightLogger.data.altitudeLast;
    _flightLogger.data.timestampTouchdown = _flightLogger.data.timestampPrevious;
    // Passed the descent touchdown altitude check, so the flight log is ended and return to GROUND
    loopStateAIRBORNEToGROUND();
    return;
  }
}

loopThrottle _throttleGround;
void loopStateGROUND(unsigned long timestamp, unsigned long deltaIn) {
  // Query the button handler to check for button press activity.
  handleButtonLoop();

  // check wifi...
  _wifi.loop();

  // Capture the command buffer.
  if (readSerial(timestamp, deltaIn))
    interpretCommandBuffer();  // TODO: It'd be nice to kick this to the other processor...

  // // Determine the ground loop time delay based on sampling rate.
  // int delta = _throttleGround.determine(timestamp, (int)SAMPLE_RATE_GROUND);
  // if (delta == 0)
  //   return;

  // // Functionality that happen on the tick goes here:

  // debug("stateGROUND...processing, delta", delta);

  // // Get the current altitude and determine the delta from initial.
  // // float altitude = 0;
  // float altitude = readSensorAltitude();
  // debug("stateGROUND...altitude", altitude);
  // debug("stateGROUND...altitudeInitial", _flightLogger.data.altitudeInitial);
  // float altitudeDelta = altitude - _flightLogger.data.altitudeInitial;
  // debug("stateGROUND...altitudeDelta", altitudeDelta);
  // _flightLogger.data.altitudeCurrent = altitude;
  // debug("stateGROUND...altitudeCurrent", _flightLogger.data.altitudeCurrent);

  // // Check for whether we've left the ground
  // // If the delta altitude is less than the specified liftoff altitude, then its on the ground.
  // // Lift altitude is a measurement of the difference between the initial altitude and current altitude.
  // debug("stateGROUND...altitudeOffsetLiftoff", altitudeOffsetLiftoff);
  // if (altitudeDelta > altitudeOffsetLiftoff) {
  //   // left the ground...
  //   debug("AIRBORNE!!!!");
  //   // Transition to the AIRBORNE_ASCENT ascent stage.
  //   loopStateGROUNDToAIRBORNE_ASCENT(timestamp);
  //   return;
  // }

  // // // If displaying the graph...
  // // if (_displayGraph) {
  // //   _drawSplash = true;
  // //   drawTftGraphForlightNbr(_flightLogger.instance.geFlightNbrLast(), _drawGraphCurveType);
  // //   return;
  // // }

  // // if (_drawSplash) {
  // //   _drawSplash = false;
  // //   drawTftSplash();
  // // }
}

void loopStateGROUNDToAIRBORNE_ASCENT(unsigned long timestamp) {
  // _loopState.current = AIRBORNE_ASCENT; // TODO!

  _displayGraph = false;

  // Turn off wifi, we don't need it in the air...
  setupNetworkDisable();

  // Re-initialize the flight...
  _flightLogger.initFlight(timestamp);
  // _flightLogger.aborted = false;
  // _flightLogger.data.altitudeLast = 0;
  // _flightLogger.data.measures = 0;
  // _flightLogger.instance.initFlight();
}

void loop() {
  // put your main code here, to run repeatedly

  unsigned long current = millis();
  unsigned long delta = current - _timestamp;

  // Simple state machine for the flight...

  // Serial.println(F("state..."));
  // Serial.println(_loopState.current);
  // Ground state
  if (_loopState.current == GROUND) {
    // Serial.println("state...GROUND");
    // Only blink while on the ground!
    _neoPixelBlinker.blink(current, 500);
    // Run the ground state algorithms
    loopStateGROUND(current, delta);
  }
  if (_loopState.current == AIRBORNE_ASCENT) {
    // Serial.println("state...AIRBORNE_ASCENT");
    // Only blink while on the ground!
    _neoPixelBlinker.off();
    // Run the airborne ascent state algorithms
    loopStateAIRBORNE_ASCENT(current, delta);
  }
  if (_loopState.current == AIRBORNE_DESCENT) {
    // Serial.println("state...AIRBORNE_DESCENT");
    // Only blink while on the ground!
    _neoPixelBlinker.off();
    // Run the airborne descent state algorithms
    loopStateAIRBORNE_DESCENT(current, delta);
  }

  _timestamp = current;
}

void setupFlightLogger() {
  Serial.println("Setup flight logger...");

  Serial.println("Setup file system...");

  if (!_flightLogger.instance.initFileSystem()) {
    Serial.println("Failed to initialize file system");
    return;
  }

  Serial.println("...file system successful.");

  Serial.println("Initialize flight logger...");

  _flightLogger.instance.initFlight();

  Serial.println("...flight logger successful.");
}

void setupFlightLoggerInitialAtmosphere() {
  Serial.println("Setup initial atmosphere...");
  
  atmosphereValues values = initializeSensorsAtmosphere();
  _flightLogger.data.altitudeInitial = values.altitude;
  Serial.print("initial altitude: ");
  Serial.println(_flightLogger.data.altitudeInitial);

  _flightLogger.data.pressureInitial = values.pressure;
  Serial.print("initial pressure: ");
  Serial.println(_flightLogger.data.pressureInitial);

  _flightLogger.data.temperatureInitial = values.temperature;
  Serial.print("initial temperature: ");
  Serial.println(_flightLogger.data.temperatureInitial);

  Serial.println("...initial atmosphere initialized.");
}

// void setupNetwork() {
//   _ble.start();
//   _wifi.start();
// }

void setupNetworkDisable() {
  // _ble.disable();
  _wifi.disable();
}

void setupTft() {
  Serial.println("Setup TFT...");

  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  _tft.init();
  _tft.fillScreen(TFT_BLACK);
  _tft.setSwapBytes(true);

  drawTftSplash();

  Serial.println("...TFT successful.");
}

void setup() {
  // put your setup code here, to run once:
  Serial.println(F("Setup..."));

  setupLedBuiltin();

  setupNeoPixelBlinker();

  turnOnLedBuiltin();

  Wire.begin();
  Serial.begin(SERIAL_BAUD);

  setupFlightLogger();

  setupTft();

  setupSensors();

  setupFlightLoggerInitialAtmosphere();

  setupSensorsCompleted();

  setupButton();

  // Setup WiFi so it's available...
  _wifi.start();

  // Disable Network
  setupNetworkDisable();

  turnOffLedBuiltin();

  drawTftSplash();

  Serial.println(F(""));
  Serial.print(BOARD_FIRMWARE);

  Serial.print(F(", version: "));
  Serial.print(MAJOR_VERSION);
  Serial.print(F("."));
  Serial.print(MINOR_VERSION);
#if defined(DEV) || defined(DEV_SIM)
  Serial.println(F(" (DEV)"));
#endif

  Serial.println(F("...finished."));
  Serial.println(F(""));
}