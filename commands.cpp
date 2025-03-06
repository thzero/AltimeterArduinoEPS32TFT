#include <ArduinoJson.h>

#include "commands.h"
#include "constants.h"
#include "debug.h"
#include "flightLogger.h"
#include "i2cscanner.h"
#include "sensor.h"
#include "simulation.h"
#include "utilities.h"

int _readIndex;
char commandbuffer[100];

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
    Serial.println(F("h;\tHelp menu"));
#ifdef DEV
    Serial.println(F("i;\tI2C scanner"));
#endif
#ifdef DEV_SIM
    Serial.println(F("s;\tsimulation"));
    Serial.println(F("st;\tsimulation stop"));
#endif
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
    if (command1 == 't') {
      _simulation.stop();
      return;
    }

    _simulation.start(simulationConfigDefault, _flightLogger.altitudeInitial);
}

//
//    Available commands.
//    The commands can be used via the serial command line or via the Android console

//    a  get all flight data
//    e  erase all saved flights
//    h  help
//    l  list all flights
//    n  the number of recorded flights
//    r  followed by a number which is the flight number
//       retrieve all data for the specified flight
//    s  start simulation
//    t  toggle flight telemetry
//    s  stop simulation
//    x  delete last recorded flight
//  
void interpretCommandBufferI() {
  char command = commandbuffer[0];
  char command1 = commandbuffer[1];

// #ifdef DEBUG
//   debug(F("interpretCommandBufferI.command"), command);
//   Serial.print(F("interpretCommandBufferI.commandBuffer="));
//   for (int i = 0; i < _readIndex; i++) {
//     Serial.print(commandbuffer[i]);
//     // Serial.printf("%c", commandbuffer[i]);
//     // Serial.printf("%d", commandbuffer[i]);
//   }
//   Serial.println(F(""));
// #endif  

#ifdef DEBUG
    Serial.println(F(""));
    Serial.println(F(""));
    Serial.println(F(""));
    Serial.println(F(""));
    Serial.println(F(""));
#endif

  // get all flight data
  // if (command == 'a') {
  //   handleFightPrintData(commandbuffer);
  //   return;
  // }
  // erase all flight
  // if (command == 'e')
  // {
  //   handeFlightErase(commandbuffer);
  //   return;
  // }
  if (command == 'h') {
    interpretCommandBufferHelp();
    return;
  }
#ifdef DEV
  if (command == 'i') {
    interpretCommandBufferI2CScanner();
    return;
  }
#endif
  // // list all flights
  // if (command == 'l')
  // {
  //   // logger.printFlightList();
  //   return;
  // }
#ifdef DEV_SIM
  else if (command == 's') {
    interpretCommandBufferSimulation(command1);
    return;
  }
#endif
  // number of flight
  // if (command == 'n')
  // {
  //   handleFlightList(commandbuffer);
  //   return;
  // }
  // read one flight
  // if (command == 'r')
  // {
  //   handleFlightRead(commandbuffer);
  //   return;
  // }
  // telemetry on/off
  if (command == 't')
  {
    StaticJsonDocument<200> doc;
    char json[] = "{}";
    DeserializationError error = deserializeJson(doc, json);
    Serial.println(F("\tsdfsdfds....1"));
    serializeJson(doc, Serial);
    Serial.println(F(""));
    Serial.println(F("\tsdfsdfds....2"));
    JsonArray flightLogs = doc.createNestedArray("flightLogs");
    Serial.println(F("\tsdfsdfds....3"));
    _flightLogger.instance.getFlightsAsJson(flightLogs);
    serializeJson(doc, Serial);
    Serial.println(F(""));
    Serial.println(F("\tsdfsdfds....4"));
    // not implemented
    return;
  }
  // recording
  if (command == 'w')
  {
    // readSensorAltitude();
    return;
  }
  // delete last flight
  if (command == 'x')
  {
    /*logger.eraseLastFlight();
      logger.readFlightList();
      long lastFlightNbr = _flightLogger.instance.geFlightNbrLast();
      if (lastFlightNbr < 0)
      {
      currentFileNbr = 0;
      currentMemaddress = 201;
      }
      else
      {
      currentMemaddress = logger.getFlightStop(lastFlightNbr) + 1;
      currentFileNbr = lastFlightNbr + 1;
      }*/
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
// #ifdef DEBUG
//   Serial.print(F("interpretCommandBuffer.commandBuffer="));
//   for (int i = 0; i < _readIndex; i++) {
//     Serial.print(commandbuffer[i]);
//     // Serial.printf("%c", commandbuffer[i]);
//     // Serial.printf("%d", commandbuffer[i]);
//   }
//   Serial.println(F(""));
// #endif

  interpretCommandBufferI();
  resetCommandBuffer();
}

// void handeFlightErase(char *commandbuffer) {
//     Serial.println(F("Erase\n"));
//     _flightLogger.instance.clearFlightList();
// }

// void handleFlightList(char *commandbuffer) {
//     Serial.print(F("$start;\n"));

//     char flightData[30] = "";
//     char temp[9] = "";
//     strcat(flightData, "nbrOfFlight,");
//     sprintf(temp, "%i,", (int)_flightLogger.instance.geFlightNbrLast() + 1 );
//     strcat(flightData, temp);
//     unsigned int chk = msgChk(flightData, sizeof(flightData));
//     sprintf(temp, "%i", chk);
//     strcat(flightData, temp);
//     strcat(flightData, ";\n");

//     Serial.print(F("$"));
//     Serial.print(flightData);
//     Serial.print(F("$end;\n"));
// }

// void handleFightPrintData(char *commandbuffer) {
//     Serial.print(F("$start;\n"));
//     int i;
//     // todo
//     for (i = 0; i < _flightLogger.instance.geFlightNbrLast() + 1; i++)
//       _flightLogger.instance.printFlightData(i);
//     Serial.print(F("$end;\n"));
// }

// void handleFlightRead(char *commandbuffer) {
//     char temp[3];
//     temp[0] = commandbuffer[1];
//     if (commandbuffer[2] != '\0')
//     {
//       temp[1] = commandbuffer[2];
//       temp[2] = '\0';
//     }
//     else
//       temp[1] = '\0';

//     if (atol(temp) > -1)
//     {
//       Serial.print(F("$start;\n"));

//       _flightLogger.instance.printFlightData(atoi(temp));

//       Serial.print(F("$end;\n"));
//     }
//     else
//       Serial.println(F("not a valid flight"));
// }

bool readSerial(unsigned long timestamp, unsigned long delta) {
  char readVal = ' ';

  // lookup to get data from serial port...
  while (Serial.available()) {
    readVal = Serial.read();
    // debug(F("readSerial.readVal"), readVal);

    if (readVal == ';') {
      commandbuffer[_readIndex++] = '\0';

// #ifdef DEBUG
      // Serial.print(F()"readSerial.commandBuffer.final="));
      // for (int i = 0; i < _readIndex; i++)
      //   Serial.print(commandbuffer[i]);
      Serial.println(F(""));
// #endif
      return true;
    }

    if (readVal != '\n') {
      // debug(F("readSerial._readIndex="), _readIndex);
      if (_readIndex >= sizeof(commandbuffer)) {
        Serial.println(F("Command buffer overflow, resetting..."));
        resetCommandBuffer();
        return false;
      }

      commandbuffer[_readIndex++] = readVal;

// #ifdef DEBUG
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