#include "commands.h"
#include "constants.h"
#include "debug.h"
#include "flightLogger.h"
#include "i2cscanner.h"
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

    _simulation.start(simulationConfigDefault, _flightLogger.data.altitudeInitial);
}

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

  // write altimeter config
  if (command == 'h') {
    interpretCommandBufferHelp();
  }
#ifdef DEV
  else if (command == 'i') {
    interpretCommandBufferI2CScanner();
  }
#endif
#ifdef DEV_SIM
  else if (command == 's') {
    interpretCommandBufferSimulation(command1);
  }
#endif
  else {
    Serial.print(F("$UNKNOWN command: "));
    Serial.println(commandbuffer);
  }
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

/*
   Print altimeter config to the Serial line
*/
void printAltimeterConfig(char *altiName) {
  char altiConfig[160] = "";
  char temp[25] = "";

  strcat(altiConfig, "alticonfig,");

  char format[3] = "%i";

  //Unit
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //beepingMode
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //output1
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //output2
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //output3
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //supersonicYesNo
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //mainAltitude
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //AltimeterName
  strcat(altiConfig, BOARD_FIRMWARE);
  strcat(altiConfig, ",");
  //alti major version
  sprintf(temp, format, MAJOR_VERSION);
  strcat(altiConfig, temp);
  //alti minor version
  sprintf(temp, format, MINOR_VERSION);
  strcat(altiConfig, temp);
  //output1 delay
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //output2 delay
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //output3 delay
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //Beeping frequency
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  sprintf(temp, "%lu,", 0);
  strcat(altiConfig, temp);
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //output4
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //output4 delay
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //Lift off altitude
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //Battery type
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  // recording timeout
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //altiID
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  //useTelemetryPort
  sprintf(temp, format, 0);
  strcat(altiConfig, temp);
  sprintf(temp, "%s,", altiName);
  strcat(altiConfig, temp);

  unsigned int chk = 0;
  chk = msgChk( altiConfig, sizeof(altiConfig));
  sprintf(temp, "%i;\n", chk);

  strcat(altiConfig, temp);
  Serial.print(F("$"));
  Serial.print(altiConfig);
}

// void handleAltimeterConfig(char *commandbuffer) {
//     Serial.print(F("$start;\n"));
//     printAltimeterConfig((char *)BOARD_NAME);
//     Serial.print(F("$end;\n"));
// }

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

// void handleProcessingToggle(char *commandbuffer) {
//     // if (commandbuffer[1] == '1') 
//     //   mainLoopEnable = true;
//     // else
//     //   mainLoopEnable = false;

//     Serial.print(F("$OK;\n"));
// }

// /*
//    This interprets menu commands. This can be used in the commend line or
//    this is used by the Android console

//    Commands are as folow:
//    a  get all flight data
//    b  get altimeter config
//    c  toggle continuity on and off
//    d  reset alti config
//    e  erase all saved flights
//    f  FastReading on
//    g  FastReading off
//    h  hello. Does not do much
//    i  unused
//    k  folowed by a number turn on or off the selected output
//    l  list all flights
//    m  followed by a number turn main loop on/off. if number is 1 then
//       main loop in on else turn it off
//    n  Return the number of recorded flights in the EEprom
//    o  requesting test trame
//    r  followed by a number which is the flight number.
//       This will retrieve all data for the specified flight
//    s  write altimeter config
//    t  reset alti config (why?)
//    w  Start or stop recording
//    x  delete last curve
//    y  followed by a number turn telemetry on/off. if number is 1 then
//       telemetry in on else turn it off
//    z  send gps raw data
// */
// void interpretCommandBuffer(char *commandbuffer) {
//   interpretCommandBufferI(commandbuffer);
//   memset(commandbuffer, 0, sizeof(commandbuffer));
// }

// void interpretCommandBufferI(char *commandbuffer) {
//   char command = commandbuffer[0];

//   // get all flight data
//   if (command == 'a') {
//     handleFightPrintData(commandbuffer);
//     return;
//   }
//   // get altimeter config
//   if (command == 'b')
//   {
//     handleAltimeterConfig(commandbuffer);
//     return;
//   }
//   // toggle continuity on and off
//   if (command == 'c')
//   {
//     // not implemeted
//     return;
//   }
//   // reset alti config this is equal to t why do I have 2 !!!!
//   if (command == 'd')
//   {
//     /*preferences.begin("namespace", false);
//       preferences.putString("Name", "TTGOAltimeter");
//       preferences.end();*/
//     return;
//   }
//   // erase all flight
//   if (command == 'e')
//   {
//     handeFlightErase(commandbuffer);
//     return;
//   }
//   // fast reading
//   if (command == 'f')
//   {
//     // not implemeted
//     return;
//   }
//   //FastReading off
//   if (command == 'g')
//   {
//     // not implemeted
//     return;
//   }
//   //hello
//   if (command == 'h')
//   {
//     Serial.print(F("$OK;\n"));
//     return;
//   }
//   // unused
//   if (command == 'i')
//   {
//     // not implemeted
//     return;
//   }
//   // turn on or off the selected output
//   if (command == 'k')
//   {
//     // not implemeted
//     return;
//   }
//   // list all flights
//   if (command == 'l')
//   {
//     // logger.printFlightList();
//     return;
//   }
//   // mainloop on/off
//   if (command == 'm')
//   {
//     handleProcessingToggle(commandbuffer);
//     return;
//   }
//   // number of flight
//   if (command == 'n')
//   {
//     handleFlightList(commandbuffer);
//     return;
//   }
//   // send test tram
//   if (command == 'o')
//   {
//     // not implemented
//     return;
//   }
//   // altimeter config param
//   // write  config
//   if (command == 'p')
//   {
//     // not implemented
//     return;
//   }
//   if (command == 'q')
//   {
//     Serial.print(F("$OK;\n"));
//     return;
//   }
//   // read one flight
//   if (command == 'r')
//   {
//     handleFlightRead(commandbuffer);
//     return;
//   }
//   // write altimeter config
//   if (command == 's')
//   {
//     // not implemented
//     return;
//   }
//   // reset config and set it to default
//   if (command == 't')
//   {
//     //reset config
//     /*preferences.begin("namespace", false);
//       preferences.putString("Name", "TTGOAltimeter");
//       preferences.end();*/
//     return;
//   }
//   if (command == 'v')
//   {
//     /* preferences.begin("namespace", false);
//       Serial.println(preferences.getString("Name", "TTGOAltimeter"));
//       preferences.end();*/
//     return;
//   }
//   // recording
//   if (command == 'w')
//   {
//     // recordAltitude();
//     return;
//   }
//   // delete last curve
//   if (command == 'x')
//   {
//     /*logger.eraseLastFlight();
//       logger.readFlightList();
//       long lastFlightNbr = _flightLogger.instance.geFlightNbrLast();
//       if (lastFlightNbr < 0)
//       {
//       currentFileNbr = 0;
//       currentMemaddress = 201;
//       }
//       else
//       {
//       currentMemaddress = logger.getFlightStop(lastFlightNbr) + 1;
//       currentFileNbr = lastFlightNbr + 1;
//       }
//       canRecord = logger.CanRecord();*/
//     return;
//   }
//   // telemetry on/off
//   if (command == 'y')
//   {
//     // not implemented
//     return;
//   }
//   // alti Name for ESP32
//   if (command == 'z')
//   {
//     //updateAltiName(commandbuffer);
//     Serial.print(F("$OK;\n"));
//     return;
//   }
//   // empty command
//   if (command == ' ')
//   {
//     Serial.print(F("$K0;\n"));
//     return;
//   }

//   Serial.print(F("$UNKNOWN;"));
//   Serial.println(commandbuffer[0]);
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