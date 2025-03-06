#include <Arduino.h>
#include <AsyncTCP.h>
#include <driver/timer.h>
#include <HTTPClient.h>
#include <LittleFS.h>

#include "constants.h"
#include "debug.h"
#include "fileSystem.h"
#include "flightLogger.h"
#include "monitor.h"
#include "stateMachine.h"
#include "time.h"
#include "web.h"
#include "wifi.h"

web::web() {
}

void web::disable() {
  if (_server == NULL)
    return;
    
  _server->end();
}

void web::loop() {
}

void web::setup() {
  Serial.println(F("Setup network web server..."));

  _server = new AsyncWebServer(80);

  Serial.println(F("...network web server setup successful."));
}

void web::start() {
  Serial.println(F("Start network web server..."));

  configure();

  _server->begin();

  Serial.println(F("...network web server started successful."));
}

void web::configure() {
  _server->onNotFound([this](AsyncWebServerRequest * request) {
    this->serverNotFound(request);
  });

  _server->onFileUpload([this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    this->serverHandleUpload(request, filename, index, data, len, final);
  });

  _server->on("/", HTTP_GET, [this](AsyncWebServerRequest * request) {
    #ifdef DEBUG
      Serial.println(F("\twebserver request...home"));
    #endif
    
    // if (server_authenticate(request)) {
    //   ESP_LOGD(TAG," Auth: Success");
    // Serial.println(F("\twebserver request /"));
    // request->send(200, "text/plain", "Hello, world");

    // request->send(LittleFS, "/index.html", "text/html", false, [this](const String& var) {
    //   return this->serverProcessor(var);
    // });
    request->send(LittleFS, "/index.html", "text/html", false, NULL);
  });

  _server->on("/script.js", HTTP_GET, [this](AsyncWebServerRequest * request) {
    #ifdef DEBUG
      Serial.println(F("\twebserver request...script.js"));
    #endif

    request->send(LittleFS, "/script.js", "text/javascript", false, NULL);
  });

  // Route to load data JSON
  _server->on("/data", HTTP_GET, [this](AsyncWebServerRequest *request) {
    #ifdef DEBUG
      Serial.println(F("\twebserver request...data"));
    #endif

    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonObject responseResult = response->getRoot().to<JsonObject>();
    this->jsonAtmosphere(responseResult);
    this->jsonHeader(responseResult);
    this->jsonLaunch(responseResult, false);
    this->jsonMonitor(responseResult);
    this->jsonSamples(responseResult, false);
    this->jsonWifi(responseResult);
    response->setLength();
    request->send(response);
  });

  // _server->on("^\\/flightLogs\\/([0-9]+)$", HTTP_GET, [](AsyncWebServerRequest * request) {
  _server->on("/flightLog", HTTP_GET, [](AsyncWebServerRequest * request) {
    // String flightNbrStr = request->pathArg(0);
    String flightNbrStr = request->getParam("number")->value();
    int flightNbr;
    sscanf(flightNbrStr.c_str(), "%d", &flightNbr);
    #ifdef DEBUG
      Serial.print(F("\twebserver request...flightLogs download #"));
      Serial.print(flightNbrStr);
      Serial.print(F(", "));
      Serial.println(flightNbr);
    #endif

    bool exists = _flightLogger.instance.exists(flightNbr);
    if (exists) {
      AsyncJsonResponse *response = new AsyncJsonResponse();
      JsonObject responseResult = response->getRoot().to<JsonObject>();
      flightDataReadResultsStruct flightLogResult = _flightLogger.instance.readFileAsJson(flightNbr);
      responseResult["success"] = true;
      #ifdef DEBUG
      Serial.print(F("\twebserver request...flightLogs download data #"));
      Serial.println(flightNbr);
      serializeJson(responseResult, Serial);
      Serial.println(F(""));
      #endif
      char szBuf[80];
      sprintf(szBuf, "attachment; filename=flight%s.json", flightNbr);
      response->addHeader("Content-Disposition", szBuf);
      response->addHeader("Connection", "close");
      response->setLength();
      request->send(response);
    }
    else {
      Serial.print(F("\twebserver request...flightLogs download #"));
      Serial.print(flightNbr);
      Serial.println(F(" - not found"));
      request->send(400, "text/plain", "ERROR : no flight log found");
    }
    
    #ifdef DEBUG
      Serial.print(F("\twebserver request...flightLogs download #"));
      Serial.print(flightNbr);
      Serial.println(F(" - finished"));
    #endif
  });

  // Route to load flightLogs JSON
  _server->on("/flightLogs", HTTP_GET, [this](AsyncWebServerRequest *request) {
    #ifdef DEBUG
      Serial.println(F("\twebserver request...flightLogs"));
    #endif

    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonObject responseResult = response->getRoot().to<JsonObject>();
    JsonArray flightLogs = responseResult.createNestedArray("flightLogs");
    _flightLogger.instance.listAsJson(flightLogs);
    responseResult["success"] = true;
    #ifdef DEBUG
    Serial.println(F("\twebserver request...flightLogs data"));
    serializeJson(responseResult, Serial);
    Serial.println(F(""));
    #endif
    response->setLength();
    request->send(response);
    
    #ifdef DEBUG
      Serial.println(F("\twebserver request...flightLogs - finished"));
    #endif
  });

  // Route to load settings data JSON
  _server->on("/settings/data", HTTP_GET, [this](AsyncWebServerRequest *request) {
    #ifdef DEBUG
      Serial.println(F("\twebserver request...settings data"));
    #endif

    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonObject responseResult = response->getRoot().to<JsonObject>();
    this->jsonHeader(responseResult);
    this->jsonLaunch(responseResult, true);
    this->jsonSamples(responseResult, true);
    this->jsonWifi(responseResult);

    #ifdef DEBUG
    Serial.println(F(""));
    serializeJson(responseResult, Serial);
    Serial.println(F(""));
    #endif

    response->setLength();
    request->send(response);
  });

  _server->on("/settings", HTTP_GET, [this](AsyncWebServerRequest * request) {
    #ifdef DEBUG
      Serial.println(F("\twebserver request...settings"));
    #endif

    // if (server_authenticate(request)) {
    //   ESP_LOGD(TAG," Auth: Success");
    // request->send(LittleFS, "/settings.html", "text/html", false, [this](const String& var) {
    //   return this->serverProcessor(var);
    // });
    request->send(LittleFS, "/settings.html", "text/html", false, NULL);
  });

  AsyncCallbackJsonWebHandler *handlerReset = new AsyncCallbackJsonWebHandler("/settings/reset");
  handlerReset->setMethod(HTTP_POST);
  handlerReset->onRequest([](AsyncWebServerRequest *request, JsonVariant &json) {
    #ifdef DEBUG
      Serial.println(F("\twebserver request...settings reset"));
    #endif

    #ifdef DEBUG
      Serial.println(F("\twebserver request...settings reset - state"));
    #endif
    _stateMachine.reset();
    #ifdef DEBUG
      Serial.println(F("\twebserver request...settings reset - state completed"));
    #endif
    #ifdef DEBUG
      Serial.println(F("\twebserver request...settings reset - wifi"));
    #endif
    _wifi.reset();
    #ifdef DEBUG
      Serial.println(F("\twebserver request...settings reset - wifi completed"));
    #endif

    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonObject responseResult = response->getRoot().to<JsonObject>();
    response->setLength();
    request->send(response);
  });
  _server->addHandler(handlerReset);

  AsyncCallbackJsonWebHandler *handlerSave = new AsyncCallbackJsonWebHandler("/settings/save");
  handlerSave->setMethod(HTTP_POST);
  handlerSave->onRequest([](AsyncWebServerRequest *request, JsonVariant &json) {
    #ifdef DEBUG
      Serial.println(F("\twebserver request...settings save - start"));
    #endif

    #ifdef DEBUG
    Serial.println(F("\twebserver request...settings inbound data"));
    serializeJson(json, Serial);
    Serial.println(F(""));

    const char* wifiPassword = json["wifiPassword"];
    debug("wifiPassword", wifiPassword);
    const char* wifiSSID = json["wifiSSID"];
    debug("wifiSSID", wifiSSID);
    int launchDetect = json["launchDetect"];
    debug("launchDetect", launchDetect);
    int samplesAscent = json["samplesAscent"];
    debug("samplesAscent", samplesAscent);
    int samplesDescent = json["samplesDescent"];
    debug("samplesDescent", samplesDescent);
    int samplesGround = json["samplesGround"];
    debug("samplesGround", samplesGround);
    #endif

    #ifdef DEBUG
      Serial.println(F("\twebserver request...settings save - state"));
    #endif
    _stateMachine.save(launchDetect, samplesAscent, samplesDescent, samplesGround);
    #ifdef DEBUG
      Serial.println(F("\twebserver request...settings save - state completed"));
    #endif
    #ifdef DEBUG
      Serial.println(F("\twebserver request...settings save - wifi"));
    #endif
    _wifi.save(wifiPassword, wifiSSID);
    #ifdef DEBUG
      Serial.println(F("\twebserver request...settings save - wifi completed"));
    #endif

    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonObject responseResult = response->getRoot().to<JsonObject>();
    responseResult["success"] = true;
    response->setLength();
    request->send(response);
    
    #ifdef DEBUG
      Serial.println(F("\twebserver request...settings save - finished"));
    #endif
  });
  _server->addHandler(handlerSave);

  AsyncCallbackJsonWebHandler *handlerRequestTime = new AsyncCallbackJsonWebHandler("/settings/requestTime");
  handlerRequestTime->setMethod(HTTP_POST);
  handlerRequestTime->onRequest([](AsyncWebServerRequest *request, JsonVariant &json) {
    #ifdef DEBUG
      Serial.println(F("\twebserver request...requestTime - start"));
    #endif

    #ifdef DEBUG
    Serial.println(F("\twebserver request...requestTime data"));
    serializeJson(json, Serial);
    Serial.println(F(""));
    #endif

    unsigned long epochS = json["epochS"];
    debug("epochS", epochS);
    int epochMs = json["epochMs"];
    debug("epochMs", epochMs);
    int hours = json["hours"];
    debug("hours", hours);
    int minutes = json["minutes"];
    debug("minutes", minutes);
    int seconds = json["seconds"];
    debug("seconds", seconds);
    int month = json["month"];
    debug("month", month);
    int day = json["day"];
    debug("day", day);
    int year = json["year"];
    debug("year", year);

    setTime(epochS);
			
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonObject responseResult = response->getRoot().to<JsonObject>();
    responseResult["success"] = true;
    #ifdef DEBUG
    Serial.println(F("\twebserver request...requestTime data"));
    serializeJson(responseResult, Serial);
    Serial.println(F(""));
    #endif
    response->setLength();
    request->send(response);
    
    #ifdef DEBUG
      Serial.println(F("\twebserver request...requestTime - finished"));
    #endif
  });
  _server->addHandler(handlerRequestTime);

  // Route to load style.css file
  _server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println(F("\twebserver request /style.css"));
    request->send(LittleFS, "/style.css", "text/css");
  });

  // Route to load logo.png file
  _server->on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request) {
      Serial.println(F("\twebserver request /logo.png"));
    // //request->send_P(200, "image/png", thzero_altimeters128x128, NULL);
    // const size_t imageSize = sizeof thzero_altimeters128x128;
    // AsyncWebServerResponse *response = request->beginChunkedResponse("image/bmp", [](uint8_t *buffer, size_t maxLen, size_t alreadySent) -> size_t {
    //   if (imageSize - alreadySent >= maxLen) {
    //     memcpy(buffer, thzero_altimeters128x128 + alreadySent, maxLen);
    //     return maxLen;
    //   }
    //   memcpy(buffer, thzero_altimeters128x128 + alreadySent, imageSize - alreadySent);
    //   return imageSize - alreadySent; 
    // });
    // request->send(response);
    request->send(LittleFS, "/logo.png", "image/png");
  });

  // Route to load battery.png file
  _server->on("/battery.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println(F("\twebserver request /battery.png"));
    request->send(LittleFS, "/battery.png", "image/png");
  });

  // Route to load transparent.png file
  _server->on("/transparent.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println(F("\twebserver request /transparent.png"));
    request->send(LittleFS, "/transparent.png", "image/png");
  });
}

void web::serverNotFound(AsyncWebServerRequest *request) {
  // ESP_LOGD(TAG,"Client: %s %s",request->client()->remoteIP().toString().c_str(), request->url().c_str());
  request->send(404, "text/plain", "Not found");
}
  
// // used by server.on functions to discern whether a user has the correct httpapitoken OR is authenticated by username and password
// bool server_authenticate(AsyncWebServerRequest * request) {
//   bool isAuthenticated = false;
//   if (request->authenticate(config.httpuser.c_str(), config.httppassword.c_str())) {
//     ESP_LOGD(TAG,"is authenticated via username and password");
//     isAuthenticated = true;
//     }
//   return isAuthenticated;
//   }

void web::serverHandleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (filename.endsWith(".bin") ) {
    // .bin files uploaded are processed as application firmware updates
    serverHandleUploadOTAUpdate(request, filename, index, data, len, final);
    return;
  }

  // non .bin files are uploaded to the LITTLEFS partition
  size_t freeBytes = LittleFS.totalBytes() - LittleFS.usedBytes();
  if (len < freeBytes) {
    serverHandleUploadLittleFS(request, filename, index, data, len, final);
    return;
  }

  char szBuf[80];
  sprintf(szBuf, "Cannot upload file size %d bytes, as LittleFS free space = %d bytes", len, freeBytes);
  Serial.println(szBuf);
  request->send(404, "text/plain", "Not enough free space in LittleFS partition.");
}

// handles non .bin file uploads to the LITTLEFS directory
void web::serverHandleUploadLittleFS(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  // make sure authenticated before allowing upload
  // if (serverAuthenticate(request)) {
  //   Serial.println("Auth: Failed");
  //   return request->requestAuthentication();
  // }

  char szBuf[80];
  sprintf(szBuf, "Client: %s %s",request->client()->remoteIP().toString().c_str(), request->url().c_str());
  Serial.println(szBuf);

  // open the file on first call
  if (index == 0) {
    sprintf(szBuf, "Upload Start : %s", filename.c_str());
    Serial.println(szBuf);

    filename = "/" + filename;
    if (LittleFS.exists(filename)) {
      bool res = LittleFS.remove(filename);
      sprintf(szBuf, "Delete file %s : %s", filename, res == false ? "Error" : "OK");
      Serial.println(szBuf);
    }

    fileLittleFS = LittleFS.open(filename, FILE_WRITE);
  }

  if (len) {
    // stream the incoming chunk to the opened file
    feedWatchdog();
    fileLittleFS.write(data, len);
    sprintf(szBuf, "Writing file : %s, index = %d, len = %d", filename.c_str(), index, len);
    Serial.println(szBuf);
  }

  if (final) {
    sprintf(szBuf, "Upload Complete : %s, size = %d", filename.c_str(), index+len);
    Serial.println(szBuf);
    
    // close the file handle after upload
    fileLittleFS.close();
  } 
}

// handles OTA firmware update
void web::serverHandleUploadOTAUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  // // make sure authenticated before allowing upload
  // if (server_authenticate(request)) {
  // char szBuf[80];
  // sprintf(szBuf, "Client: %s %s",request->client()->remoteIP().toString().c_str(), request->url().c_str());
  // Serial.println(szBuf);
  //   ESP_LOGD(TAG,"Client: %s %s",request->client()->remoteIP().toString().c_str(), request->url().c_str());
  //   if (!index) {
  //     ESP_LOGD(TAG,"OTA Update Start : %s", filename.c_str());
  //     if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
  //       Update.printError(Serial);
  //       }
  //     }

  //   if (len) {
  //     feedWatchdog();
  //     // flashing firmware to ESP
  //     if (Update.write(data, len) != len) {
  //       Update.printError(Serial);
  //       }      
  //     ESP_LOGD(TAG,"Writing : %s index = %d len = %d", filename.c_str(), index, len);
  //     }

  //   if (final) {
  //     if (Update.end(true)) { //true to set the size to the current progress
  //       ESP_LOGD(TAG,"OTA Complete : %s, size = %d", filename.c_str(), index + len);
  //       } 
  //     else {
  //       Update.printError(Serial);
  //       }
  //     delayMs(1000);
  //     ESP.restart(); // force reboot after firmware update
  //     }
  //   } 
  // else {
  //   ESP_LOGD(TAG,"Auth: Failed");
  //   return request->requestAuthentication();
  //   }
  // }
}

// replace %var%  in webpage with dynamically generated string
// String web::serverProcessor(const String& var) {
//   // Serial.println(var);
//   char temp[23];

//   if (var == "TITLE")
//       return String(BOARD_NAME);

//   if (var == "COPYRIGHT")
//       return String(COPYRIGHT); 
//   if (var == "COPYRIGHT_YEARS")
//       return String(COPYRIGHT_YEARS); 

//   if (var == "VERSION")
//       return String(MAJOR_VERSION) + "." + String(MINOR_VERSION); 

//   if (var == "LAUNCH_DETECT") {
//     sprintf(temp, "%d", altitudeOffsetLiftoff);
//     return temp;
//     return "20"; // TODO
//   }
    
//   if (var == "MONITOR_BATTERY_VOLTAGE") {
//     return "3.5";   // TODO: determine if battery level if plugged in...
//   }
//   if (var == "MONITOR_BATTERY_VOLTAGE_MAX") {
//     return "3.7";   // TODO: determine if battery level if plugged in...
//   }

//   if (var == "MONITOR_BATTERY_STATUS")
// #ifdef MONITOR_BATTERY
//     return "true";  // TODO: determine if battery is plugged in...
// #else
//     return "false";
// #endif

//   if (var == "MONITOR_MEMORY_FREE") {
//     sprintf(temp, "%u", _monitor.heap());
//     return temp;
//   }

//   if (var == "WIFI_ADDRESS")
//     return _wifi.ipAddress();

//   if (var == "WIFI_SSID")
//     return _wifi.ssid();

//   // if (var == "TOTALDATALOG")
//   //     return server_ui_size(FLASH_SIZE_BYTES);

//   // if (var == "USEDDATALOG")
//   //     return server_ui_size(FlashLogFreeAddress);26
  
//   return "?";
// }

void web::feedWatchdog() {
  // TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  // TIMERG0.wdt_feed = 1;
  // TIMERG0.wdt_wprotect = 0;
  TIMERG0.wdtwprotect.wdt_wkey = 1356348065;
  TIMERG0.wdtfeed.wdt_feed = 1;
  TIMERG0.wdtwprotect.wdt_wkey = 0;
}

void web::jsonAtmosphere(JsonObject root) {
  root["altitudeASL"] = _flightLogger.pressureInitial;
  root["pressureAbove"] = _flightLogger.altitudeInitial;
  root["pressureAt"] = _flightLogger.pressureInitial; // TODO
  root["temperature"] = _flightLogger.temperatureInitial; // TODO
}

void web::jsonHeader(JsonObject root) {
  root["title"] = BOARD_NAME;
  root["copyright"] = COPYRIGHT;
  root["copyrightYears"] = COPYRIGHT_YEARS;
  root["version"] = String(MAJOR_VERSION) + "." + String(MINOR_VERSION);
}

void web::jsonLaunch(JsonObject root, bool settings) {
  root["launchDetect"] = _stateMachine.launchDetect();

  if (settings) {
    root["launchDetectValuesDefault"] = (int)ALTITUDE_LIFTOFF;
    JsonArray launchDetectValues = root.createNestedArray("launchDetectValues");
    _values("launchDetectValues", _stateMachine.launchDetectValues, launchDetectValues, sizeof(_stateMachine.launchDetectValues) / sizeof(_stateMachine.launchDetectValues[0]));
  }
}

void web::jsonMonitor(JsonObject root) {
#ifdef MONITOR_BATTERY
  root["monitorBatteryStatus"] = true;
#else
  root["monitorBatteryStatus"] = false;
#endif
  root["monitorBatteryVoltage"] = "3.5";
  root["monitorBatteryVoltageMax"] = "3.7";

  root["monitorMemoryFree"] = _monitor.heap();

  root["fileSystemTotalBytes"] = fileSystemTotalBytes();
  root["fileSystemUsedBytes"] = fileSystemUsedBytes();
}

void web::jsonSamples(JsonObject root, bool settings) {
  root["samplesAscent"] = _stateMachine.sampleRateAirborneAscent();
  root["samplesDescent"] = _stateMachine.sampleRateAirborneDescent();
  root["samplesGround"] = _stateMachine.sampleRateGround();

  if (settings) {
    root["sampleRateAirborneAscentValuesDefault"] = (int)SAMPLE_RATE_AIRBORNE_ASCENT;
    JsonArray sampleRateAirborneAscentValues = root.createNestedArray("sampleRateAirborneAscentValues");
    _values("sampleRateAirborneAscentValues", _stateMachine.sampleRateAirborneAscentValues, sampleRateAirborneAscentValues, sizeof(_stateMachine.sampleRateAirborneAscentValues) / sizeof(_stateMachine.sampleRateAirborneAscentValues[0]));

    root["sampleRateAirborneDescentValuesDefault"] = (int)SAMPLE_RATE_AIRBORNE_DESCENT;
    JsonArray sampleRateAirborneDecentValues = root.createNestedArray("sampleRateAirborneDecentValues");
    _values("sampleRateAirborneDecentValues", _stateMachine.sampleRateAirborneDecentValues, sampleRateAirborneDecentValues, sizeof(_stateMachine.sampleRateAirborneDecentValues) / sizeof(_stateMachine.sampleRateAirborneDecentValues[0]));
    
    root["sampleRateGroundValuesDefault"] = (int)SAMPLE_RATE_GROUND;
    JsonArray sampleRateGroundValues = root.createNestedArray("sampleRateGroundValues");
    _values("sampleRateGroundValues", _stateMachine.sampleRateGroundValues, sampleRateGroundValues, sizeof(_stateMachine.sampleRateGroundValues) / sizeof(_stateMachine.sampleRateGroundValues[0]));
  }
}

void web::jsonWifi(JsonObject root) {
  root["wifiAddress"] = _wifi.ipAddress();
  root["wifiPassword"] = _wifi.password();
  root["wifiSSID"] = _wifi.ssid();
}

void web::_values(char *name, int values[], JsonArray valuesJson, int size) {
  Serial.print("_values=");
  Serial.println(name);
  debug("values.length", size);
  for (int i = 0; i < size; i++) {
    Serial.print("_values[");
    Serial.print(i);
    Serial.print("]=");
    Serial.println(values[i]);
    valuesJson.add(values[i]);
  }
}

web _web;