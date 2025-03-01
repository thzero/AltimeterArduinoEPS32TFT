// #include "flightLogger.h"

// #define FORMAT "%i";
// #define FORMAT_COMMA "%i,";
// #define FORMAT_FLOAT "%.2f";
// #define BLANK "-1,";
// #define TELEMETRY "telemetry,";

// void sendTelemetry(long sampleTime, atmosphereValues atomosphere, accelerometerValues accelerometer, gyroscopeValues gryoscope, float altitude, int freq) {
//   if (!telemetryEnable)
//     return;
//   if ((timestamp - lastTelemetry) <= freq) {
//     lastTelemetry = timestamp;
//     return;
//   }
  
//   char altiTelem[150] = "";
//   char temp[10] = "";
//   char format[3] = "%i";
//   char formatComma[4] = "%i,";
//   char formatFloat[4] = "%.2f";
//   char formatFloatComma[5] = "%.2f,";
//   char blank[4] = "-1,";

//   int val = 0;
//   int airborne = _flightLogger.airborne ? 1 : 0;
//   // if (_flightLogger.airborne)
//   //   airborne = 1;

//   int touchdown = _flightLogger.touchdown ? 1 : 0;
//   // if (_flightLogger.touchdown)
//   //   touchdown = 1;

//   strcat(altiTelem, "telemetry,");

//   sprintf(temp, formatFloatComma, _flightLogger.instance.getFlightData().altitudeCurrent);
//   strcat(altiTelem, temp);
//   sprintf(temp, formatComma, airborne);
//   strcat(altiTelem, temp);
//   sprintf(temp, formatComma, _flightLogger.instance.getFlightData().altitudeLaunch);
//   strcat(altiTelem, temp);
//   sprintf(temp, formatComma, _flightLogger.instance.getFlightData().timestampLaunch);
//   strcat(altiTelem, temp);
//   sprintf(temp, formatFloatComma, _flightLogger.instance.getFlightData().altitudeApogee);
//   strcat(altiTelem, temp);
//   sprintf(temp, formatFloatComma, _flightLogger.instance.getFlightData().timestampApogee);
//   strcat(altiTelem, temp);
//   sprintf(temp, formatComma, -1);
//   strcat(altiTelem, temp);
//   sprintf(temp, formatComma, touchdown);
//   strcat(altiTelem, temp);
//   sprintf(temp, formatComma, _flightLogger.instance.getFlightData().timestampTouchdown);
//   strcat(altiTelem, temp);
//   sprintf(temp, formatComma, timestamp);
//   strcat(altiTelem, temp);

//   strcat(altiTelem, blank);
//   strcat(altiTelem, blank);
//   strcat(altiTelem, blank);
//   strcat(altiTelem, blank);

//   // dtostrf(BL.getBatteryVolts(), 4, 2, temp);
//   // strcat(altiTelem, temp);
//   strcat(altiTelem, blank);

//   sprintf(temp, formatFloat, atomosphere.temperature);
//   strcat(altiTelem, temp);
//   sprintf(temp, formatFloat, atomosphere.pressure);
//   strcat(altiTelem, temp);

//   strcat(altiTelem, blank);

//   sprintf(temp, formatComma, flightLogger.getLastFlightNbr() + 1);
//   strcat(altiTelem, temp);

//   // drogueFiredAltitude
//   sprintf(temp, formatComma, -1);
//   strcat(altiTelem, temp);

//   if (qmi.getDataReady()) {
//     if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
//       sprintf(temp, formatComma, (int)(1000 * accelerometer.x));
//       strcat(altiTelem, temp);
//       sprintf(temp, formatComma, (int)(1000 * accelerometer.y));
//       strcat(altiTelem, temp);
//       sprintf(temp, formatComma, (int)(1000 * accelerometer.z));
//     } else {
//       sprintf(temp, formatComma, -1);
//       strcat(altiTelem, temp);
//       sprintf(temp, formatComma, -1);
//       strcat(altiTelem, temp);
//       sprintf(temp, formatComma, -1);
//     }
//   } else {
//     sprintf(temp, formatComma, -1);
//     strcat(altiTelem, temp);
//     sprintf(temp, formatComma, -1);
//     strcat(altiTelem, temp);
//     sprintf(temp, formatComma, -1);
//   }
//   strcat(altiTelem, temp);

//   unsigned int chk;
//   chk = msgChk(altiTelem, sizeof(altiTelem));
//   sprintf(temp, format, chk);
//   strcat(altiTelem, temp);
//   strcat(altiTelem, ";\n");

//   Serial.print(F("$"));
//   Serial.print(altiTelem);
  
//   lastTelemetry = timestamp;
// }