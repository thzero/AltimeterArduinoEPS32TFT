#include <Button2.h>
#include <driver/rtc_io.h>

#include "button.h"

long _drawGraphCurveType = 0;
long _drawGraphFlightNbr = 0;

bool _displayGraph;

Button2 btn(BUTTON_GPIO); // Initialize the down button

void handleButtonLoop() {
  btn.loop();
}

void handleButtonClick() {
  Serial.println(F("handleButtonClick!!!!"));
//     if (!_displayGraph)
//       return;

//     long lastFlightNbr = _flightLogger.instance.geFlightNbrsLast();
//     _drawGraphCurveType++;
//     // Make sure we have not reach the last flight
//     if ((lastFlightNbr > _drawGraphFlightNbr) & (_drawGraphCurveType > 4) ) {
//       _drawGraphFlightNbr ++;
//       _drawGraphCurveType = 0;
//     } 
//     else {
//       // if not lets go back to the first one if it exists
//       if (!((lastFlightNbr < 1))) {
//         if (_drawGraphCurveType > 4) {
//           _drawGraphFlightNbr = 1;
//           _drawGraphCurveType = 0;
//         }
//       }
//     }

// #ifdef DEBUG
//     Serial.print(F("Flight:"));
//     Serial.println(_drawGraphFlightNbr);
//     Serial.print(F("type:"));
//     Serial.println(_drawGraphCurveType);
// #endif

//     drawTftGraphFlightNbr(_drawGraphFlightNbr, _drawGraphCurveType);
}

void handleButtonLongClick_Display() {
//   Serial.println(F("handleButtonLongClick_Display!!!!"));
//    if (!_displayGraph) {
//     long lastFlightNbr = _flightLogger.instance.geFlightNbrsLast();
// #ifdef DEBUG
//     Serial.print(F("lastFlightNbr:"));
//     Serial.println(lastFlightNbr);
// #endif
//     if (!(lastFlightNbr < 1)) {
//       _displayGraph = true;
//       _drawGraphFlightNbr = 1;
//       drawTftGraphFlightNbr(_drawGraphFlightNbr, _drawGraphCurveType);
//     }
//   } 
//   else {
//     _displayGraph = false;
//     _tft.init();
//     _tft.fillScreen(TFT_BLACK);

//     // Graph area is 200 pixels wide, 150 high, dark grey background
//     _graphWidget.createGraph(200, 100, _tft.color565(5, 5, 5));
//     // x scale units is from 0 to 100, y scale units is 0 to 50
//     _graphWidget.setGraphScale(0.0, 100.0, 0, 50.0);
//   }
}

void handleButtonLongClick_Exit() {
  Serial.println(F("handleButtonLongClick_Exit!!!!"));
  //Serial.println(F("Turning off"));
  // sleepDevice();
}

void handleButtonLongClick_FlightLogErase() {
  Serial.println(F("handleButtonLongClick_10!!!!"));

  // // erasing flights...
  // _displayGraph = false;
  // _tft.init();

  // _tft.fillScreen(TFT_BLACK);

  // // Graph area is 200 pixels wide, 150 high, dark grey background
  // _graphWidget.createGraph(200, 100, _tft.color565(5, 5, 5));
  // // x scale units is from 0 to 100, y scale units is 0 to 50
  // _graphWidget.setGraphScale(0.0, 100.0, 0, 50.0);
  // Serial.println(F("Erasing flights!!!"));

  // // erasing flights
  // _flightLogger.instance.clearFlights();
}

void handleButtonLongClick(Button2 button) {
  Serial.println(F("handleButtonLongClick!!!!"));
     unsigned int time = button.wasPressedFor();

    if (time >= 10000) {
      handleButtonLongClick_FlightLogErase();
      return;
    }

    // Exit
    if (time >= 5000 & time < 10000) {
      handleButtonLongClick_Exit();
      return;
    }

    if (time >= 1000 & time < 5000) {
      handleButtonLongClick_Display();
      return;
    }
}

void setupButton() {
  Serial.println(F("\nSetup button..."));

  btn.setClickHandler([](Button2 & b) {
    // Down
    Serial.println(F("Button Down fast")); // It's called upCmd because it increases the index of an array. Visually that would mean the selector goes downwards.
    handleButtonClick();
  });

  btn.setLongClickHandler([](Button2 & b) {
    Serial.println(F("Button Down slow"));
    handleButtonLongClick(b);
  });

  Serial.println(F("...button successful."));
}

void setupButtonDeninit() {
  pinMode(BUTTON_GPIO, INPUT_PULLUP);
  rtc_gpio_hold_en(BUTTON_GPIO);
  esp_sleep_enable_ext0_wakeup(BUTTON_GPIO, LOW);
}