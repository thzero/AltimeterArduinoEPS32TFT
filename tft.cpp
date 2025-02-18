#include <SPI.h>
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>

#include "constants.h"
#include "debug.h"
#include "flightLogger.h"
#include "sensor.h"
#include "simulation.h"
#include "tft.h"
#include "utilities.h"
#include "wifi.h"
#include "images/thzero_altimeters128x128.h"

TFT_eSPI _tft = TFT_eSPI();

GraphWidget _graphWidget = GraphWidget(&_tft);  // Graph widget
TraceWidget _graphTraceAltitude = TraceWidget(&_graphWidget);
TraceWidget _graphTraceTemperature = TraceWidget(&_graphWidget);
TraceWidget _graphTracePressure = TraceWidget(&_graphWidget);
TraceWidget _graphTraceAccelX = TraceWidget(&_graphWidget);
TraceWidget _graphTraceAccelY = TraceWidget(&_graphWidget);
TraceWidget _graphTraceAccelZ = TraceWidget(&_graphWidget);
TraceWidget _graphTraceHumidity = TraceWidget(&_graphWidget);

void drawTftFlightAirborne(unsigned long timestamp, unsigned long delta) {
  _tft.setCursor(0, STATUS_HEIGHT_BAR);

  if (_flightLogger.aborted) {
    _tft.println(F("ABORTED!!!"));
    return;
  }

  _tft.println(F("Recording in progress ....."));
  _tft.println(F(""));

  char altitude[40];
  _tft.println("Altitude");
  sprintf(altitude, "Initial: %.2fm", _flightLogger.data.altitudeInitial);
  _tft.println(altitude);

  sprintf(altitude, "Current: %.2fm", _flightLogger.data.altitudeCurrent + _flightLogger.data.altitudeInitial);
  _tft.println(altitude);

  drawTftSensorImu();
}

void drawTftFlightAirborneStart() {
#ifdef DEV_SIM
  _tft.fillScreen(TFT_RED);
  _tft.setTextColor(TFT_WHITE, TFT_RED);
#else
  _tft.fillScreen(TFT_BLACK);
  _tft.setTextColor(TFT_WHITE, TFT_BLACK);
#endif
  _tft.setRotation(1);
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
    _tft.drawString(F("time(ms)"), _graphWidget.getPointX(maxX / 4), _graphWidget.getPointY(0) + 3);
  } 
  else {
    char temp[10];
    sprintf(temp, "%3.1f", maxX / 1000 / 2);
    _tft.drawString(temp, _graphWidget.getPointX(maxX / 2), _graphWidget.getPointY(0) + 3);
    sprintf(temp, "%3.1f", maxX / 1000);
    _tft.drawString(temp, _graphWidget.getPointX(maxX) - 10, _graphWidget.getPointY(0) + 3);
    _tft.drawString(F("time (s)"), _graphWidget.getPointX(maxX / 4), _graphWidget.getPointY(0) + 3);
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
  Serial.println(F("Display graph on tft..."));

  if (_tft.getRotation() == 1) {
    _tft.setRotation(0);
    _tft.fillScreen(TFT_BLACK);
  }

  _tft.drawString(F("altitude"), 6, 0);
  // Graph area is 200 pixels wide, 150 high, dark grey background
  _graphWidget.createGraph(200, 100, _tft.color565(5, 5, 5));
  // x scale units is from 0 to 100, y scale units is 0 to 50
  _graphWidget.setGraphScale(0.0, 100.0, 0, 50.0);

  _tft.setTextColor(TFT_WHITE, TFT_BLACK);
  _tft.fillScreen(TFT_BLACK);
  _tft.setSwapBytes(true);
  _tft.setTextFont(2);

  Serial.println(F("...display graph on tft successful."));

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

void drawTftReset() {
  _tft.setTextColor(TFT_WHITE, TFT_BLACK);
  _tft.fillScreen(TFT_BLACK);
  _tft.setRotation(0);
}

void drawTftSensorImu() {
  accelerometerValues accelerometerValuesO = readSensorAccelerometer();
  char temp[15];
  sprintf(temp, "x=%3.2f m/s", accelerometerValuesO.x);
  _tft.println("");
  _tft.println(temp);
  sprintf(temp, "y=%3.2f m/s", accelerometerValuesO.y);
  _tft.println(temp);
  sprintf(temp, "z=%3.2f m/s", accelerometerValuesO.z);
  _tft.println(temp);
  
  // gyroscopeValues readSensorGyroscope();
}

void drawTftSleep() {
  _tft.drawString("turning off...", 6, 185);
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
  Serial.println(F("Display splash on tft..."));

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
    ipAddress.concat("    ");
    Serial.print(F("WiFi IP address: "));
    Serial.println(_wifi.ipAddress());
    _tft.drawString(ipAddress, 6, row);
  }
  else {
    _tft.drawString(F("Enable WiFi"), 6, row);
    row += 10;
    _tft.drawString(F("5 sec long Press"), 6, row);
  }
  row += 15;

  _tft.drawString(F("Alt.   Temp. Pres."), 6, row);
  row += 10;
  char temp[40];
  sprintf(temp, "%.0fm%s%.0fC%s%.0fhPa", _flightLogger.data.altitudeInitial, drawTftSplashPad(_flightLogger.data.altitudeInitial, 6, "%.0f"), _flightLogger.data.temperatureInitial, drawTftSplashPad(_flightLogger.data.temperatureInitial, 5, "%.0f"), _flightLogger.data.pressureInitial);
  _tft.drawString(temp, 6, row);
  row += 15;
  
  if (_flightLogger.aborted) {
    _tft.drawString(F("ABORTED!!!"), 6, row);
    return;
  }

  Serial.println(F("...display splash on tft successful."));
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
  drawTftReset();
  _tft.setSwapBytes(true);

  drawTftSplash();

  Serial.println("...TFT successful.");
}

void sleepTft() {
  // neilrbowen...
  pinMode(TFT_BACKLITE, OUTPUT);
  pinMode(TFT_I2C_POWER, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(TFT_BACKLITE, LOW);
  digitalWrite(TFT_I2C_POWER, LOW);
  // neilrbowen...
}

void sleepTftHold() {
  // neilrbowen...
  gpio_hold_en((gpio_num_t)TFT_BACKLITE);
  gpio_hold_en((gpio_num_t)TFT_I2C_POWER);
  // neilrbowen...
}
