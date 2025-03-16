#include <ArduinoJson.h>

#include "commands.h"
#include "constants.h"
#include "debug.h"
#include "fileSystem.h"
#include "flightLogger.h"
#include "flightLoggerData.h"
#include "i2cscanner.h"
#include "sensor.h"
#include "simulation.h"
#include "utilities.h"

int _readIndex;
char commandbuffer[100];

void handleFlightsListing(char *commandbuffer) {
  char command = commandbuffer[0];
  char command1 = commandbuffer[1];
#ifdef DEV
  if (command1 == 'j') {
    JsonDocument doc;
    deserializeJson(doc, "[]");
    JsonArray flightLogs = doc.as<JsonArray>();
    _flightLogger.instance.listAsJson(flightLogs);
    if (flightLogs && flightLogs.size() > 0) {
    serializeJson(flightLogs, Serial);
    Serial.println();
    return;
    }
    Serial.println(F("[]"));
    return;
  }
#endif
  _flightLogger.instance.outputSerialList();
}

void handleFlightsNumberNax(char *commandbuffer) {
  Serial.print(F("$start;\n"));

  char flightData[30] = "";
  char temp[9] = "";
  strcat(flightData, "nbrOfFlight,");
  sprintf(temp, "%i,", (int)_flightLogger.instance.geFlightNbrsLast());
  strcat(flightData, temp);
  unsigned int chk = msgChk(flightData, sizeof(flightData));
  sprintf(temp, "%i", chk);
  strcat(flightData, temp);
  strcat(flightData, ";\n");

  Serial.print(F("$"));
  Serial.print(flightData);
  Serial.print(F("$end;\n"));
}

void handleFlightOutputSerial(char *commandbuffer) {
  char temp[3];
  bool expanded = false;
  if (commandbuffer[1] == 'e') {
    expanded = true;
    temp[0] = commandbuffer[2];
    if (commandbuffer[3] != '\0') {
      temp[1] = commandbuffer[3];
      temp[2] = '\0';
    }
    else
      temp[1] = '\0';
  }
  else {
    temp[0] = commandbuffer[1];
    if (commandbuffer[2] != '\0') {
      temp[1] = commandbuffer[2];
      temp[2] = '\0';
    }
    else
      temp[1] = '\0';
  }
  // Serial.print(F("Flight temp: "));
  // Serial.println(temp);
  // Serial.print(F("Flight expanded: "));
  // Serial.println(expanded);

  long number = atol(temp);
  if (atol(temp) <= 0) {
    Serial.print(F("Value '"));
    Serial.print(temp);
    Serial.println(F("' is an invalid flight number."));
    return;
  }

  int last = _flightLogger.instance.geFlightNbrsLast();
  // Serial.println(F("Last flight #"));
  // Serial.println(number);
  if (number > last) {
    Serial.print(F("Flight #"));
    Serial.print(number);
    Serial.println(F(" is not a valid flight."));
    return;
  }

  Serial.print(F("$start;\n"));

  if (!expanded)
    _flightLogger.instance.outputSerial(number);
  else
    _flightLogger.instance.outputSerialExpanded(number);

  Serial.print(F("$end;\n"));
}

void handleFightsOutputSerial(char *commandbuffer) {
  bool expanded = false;
  if (commandbuffer[1] == 'e')
    expanded = true;
  // debug("expanded", expanded);

  Serial.print(F("$start;\n"));
  
  flightDataNumberStruct results = _flightLogger.instance.geFlightNbrs();
  // debug("results.size", results.size);
  for (int index = 0; index < results.size; index++) {
    int number = results.numbers[index];
    // debug("number", number);
    if (!expanded)
      _flightLogger.instance.outputSerial(number);
    else
      _flightLogger.instance.outputSerialExpanded(number);
  }
  Serial.print(F("$end;\n"));
}

void interpretCommandBufferHelp() {
  Serial.println(F(""));
  Serial.print(BOARD_NAME);
  Serial.println(F(" help..."));

  Serial.print(F("version\t"));
  Serial.print(MAJOR_VERSION);
  Serial.print(F("."));
  Serial.print(MINOR_VERSION);
#if defined(DEV) || defined(DEV_SIM)
  Serial.print(F(" (DEV)"));
#endif
  Serial.println(F(""));

  Serial.println(F("command\tdescription"));
  Serial.println(F("h;\thelp menu"));
  Serial.println(F("a;\toutput to serial all flight logs"));
  Serial.println(F("ae;\texpanded output to serial all flight logs"));
  Serial.println(F("e;\terase all recorded flights"));
#ifdef DEV
  Serial.println(F("i;\tI2C scanner"));
#endif
  Serial.println(F("l;\toutput to seriali a list of all flight logs"));
#ifdef DEV
  Serial.println(F("lj;\toutput to seriali a list of all flight logs - json"));
#endif
  Serial.println(F("n;\toutput to serial the number of recorded flight logs"));
  Serial.println(F("r<#>;\toutput to serial data for flight log <#>"));
  Serial.println(F("re<#>;\texpanded output to serial data for flight log <#>"));
#ifdef DEV_SIM
  Serial.println(F("s;\tsimulation"));
  Serial.println(F("st;\tsimulation stop"));
#endif
  // Serial.println(F("z;\ttoggle flight telemetry"));
  Serial.println(F("x;\tdelete last recorded flight log"));
  Serial.println(F("z;\treindex recorded flight log index"));
  Serial.println(F("")); 

#ifdef DEBUG
  Serial.println(F(""));
  Serial.println(F(""));
  Serial.println(F(""));
  Serial.println(F(""));
  Serial.println(F(""));
#endif
}

void interpretCommandBufferI2CScanner() {
  i2CScanner();
}

void interpretCommandBufferSimulation(char command1) {
  if (command1 == 'l') {
    _simulation.outputSerialList();
    return;
  }
  if (command1 == 't') {
    _simulation.stop();
    return;
  }

  _simulation.start(_flightLogger.altitudeInitial);
}

//  Available commands.
//  The commands can be used via the serial command line or via the Android console

//  a   output to serial all flight logs
//  e   erase all recorded flight logs
//  h   help
//  i   i2c scanner - DEV only
//  l   output to seriali a list of all flight logs
//  lj  output to seriali a list of all flight logs - json - DEV only
//  n   output to serial the number of recorded flight logs
//  r<n>  followed by a number which is the flight number
//      output to serial data for the specified flight log
//  s   start simulation - DEV only
//  st   stop simulation - DEV only
//  t   toggle flight log telemetry
//  x   delete last recorded flight log
//  z   reindex recorded flight log index
//  
void interpretCommandBufferI() {
  char command = commandbuffer[0];
  char command1 = commandbuffer[1];

#if defined(DWBUG) && defined(DEBUG_COMMAND)
  debug(F("interpretCommandBufferI.command"), command);
  Serial.print(F("interpretCommandBufferI.commandBuffer="));
  for (int i = 0; i < _readIndex; i++) {
  Serial.print(commandbuffer[i]);
  // Serial.printf("%c", commandbuffer[i]);
  // Serial.printf("%d", commandbuffer[i]);
  }
  Serial.println(F(""));
#endif  

#if defined(DWBUG) && defined(DEBUG_COMMAND)
  Serial.println(F(""));
  Serial.println(F(""));
  Serial.println(commandbuffer[0]);
  Serial.println(commandbuffer[1]);
  Serial.println(F(""));
  Serial.println(F(""));
  Serial.println(F(""));
#endif

  // output to serial all flight logs
  if (command == 'a') {
    handleFightsOutputSerial(commandbuffer);
    return;
  }
  // erase all recorded flight logs
  if (command == 'e') {
    _flightLogger.instance.eraseFlights();
    return;
  }
  // help
  if (command == 'h') {
    interpretCommandBufferHelp();
    return;
  }
#ifdef DEV
  // i2c scanner - debug only
  if (command == 'i') {
    interpretCommandBufferI2CScanner();
    return;
  }
#endif
  // output to seriali a list of all flight logs
  if (command == 'l') {
    handleFlightsListing(commandbuffer);
    return;
  }
  // output to serial the number of recorded flight logs
  if (command == 'n') {
    handleFlightsNumberNax(commandbuffer);
    return;
  }
  // output to serial data for the specified flight log
  if (command == 'r') {
    handleFlightOutputSerial(commandbuffer);
    return;
  }
#ifdef DEV_SIM
  // start simulation - DEV only
  // stop simulation - DEV only
  else if (command == 's') {
    interpretCommandBufferSimulation(command1);
    return;
  }
#endif
  // toggle flight log telemetry
  if (command == 't') {
  // not implemented
  return;
  }
  // recording
  if (command == 'w') {
  // readSensorAltitude();
  return;
  }
  // delete last recorded flight log
  if (command == 'x') {
    _flightLogger.instance.eraseLast();
    return;
  }
#ifdef DEV
  // test command
  if (command == 'y') {
    return;
  }
#endif
  // reindex recorded flight log index
  if (command == 'z') {
    _flightLogger.instance.reindexFlights();
    return;
  }
  Serial.print(F("$UNKNOWN command: "));
  Serial.println(commandbuffer);
}

void resetCommandBuffer() {
  memset(commandbuffer, 0, sizeof(commandbuffer));
  _readIndex = 0;
}

void interpretCommandBuffer() {
// #if defined(DWBUG) && defined(DEBUG_COMMAND)
//   Serial.print(F("interpretCommandBuffer.commandBuffer="));
//   for (int i = 0; i < _readIndex; i++) {
//   Serial.print(commandbuffer[i]);
//   // Serial.printf("%c", commandbuffer[i]);
//   // Serial.printf("%d", commandbuffer[i]);
//   }
//   Serial.println(F(""));
// #endif

  interpretCommandBufferI();
  resetCommandBuffer();
}

bool readSerial(unsigned long timestamp, unsigned long delta) {
  char readVal = ' ';

  // lookup to get data from serial port...
  while (Serial.available()) {
    readVal = Serial.read();
// #if defined(DWBUG) && defined(DEBUG_COMMAND)
  // debug(F("readSerial.readVal"), readVal);
// #endif

    if (readVal == ';') {
      commandbuffer[_readIndex++] = '\0';

// #if defined(DWBUG) && defined(DEBUG_COMMAND)
    // Serial.print(F()"readSerial.commandBuffer.final="));
    // for (int i = 0; i < _readIndex; i++)
    //   Serial.print(commandbuffer[i]);
    // Serial.println(F(""));
// #endif
      return true;
    }

    if (readVal != '\n') {
// #if defined(DWBUG) && defined(DEBUG_COMMAND)
    // debug(F("readSerial._readIndex="), _readIndex);
// #endif
      if (_readIndex >= sizeof(commandbuffer)) {
        Serial.println(F("Command buffer overflow, resetting..."));
        resetCommandBuffer();
        return false;
      }

      commandbuffer[_readIndex++] = readVal;

// #if defined(DWBUG) && defined(DEBUG_COMMAND)
    // Serial.print(F()"readSerial.commandBuffer="));
    // for (int i = 0; i < _readIndex; i++) {
    //   Serial.print(commandbuffer[i]);
    // }
    // Serial.println(F(""));
// #endif
    }
  }

  return false;
}