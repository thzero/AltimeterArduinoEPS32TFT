#include <Arduino.h>
#include <Arduino_JSON.h>
#include <AsyncTCP.h>
#include <HTTPClient.h>
#include <LittleFS.h>

#include "debug.h"
#include "web.h"

const char HTML[] PROGMEM = " \
<html> \
  <body> \
    hi \
  </body> \
</html>";

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

  // // Route for root / web page
  // _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(LittleFS, "/index.html", String(), false, processor);
  // });
  // _server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send_P(200, "text/plain", temperature.c_str());
  // });
  // _server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send_P(200, "text/plain", humidity.c_str());
  // });
  // _server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send_P(200, "text/plain", pressure.c_str());
  // });
  // _server.on("/timestamp", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send_P(200, "text/plain", timestamp.c_str());
  // });
  // _server.on("/rssi", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send_P(200, "text/plain", String(rssi).c_str());
  // });
  // _server.on("/winter", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(LittleFS, "/winter.jpg", "image/jpg");
  // });

  // // Start server
  // server.begin();

  Serial.println(F("...network web server started successful."));
}

void web::configure() {
  _server->onNotFound([this](AsyncWebServerRequest * request) {
    this->serverNotFound(request);
  });
  // server->onFileUpload(server_handle_upload);
  _server->on("/", HTTP_GET, [this](AsyncWebServerRequest * request) {
    // ESP_LOGD(TAG,"Client: %s %s",request->client()->remoteIP().toString().c_str(), request->url().c_str());
    // if (server_authenticate(request)) {
    //   ESP_LOGD(TAG," Auth: Success");
      Serial.println(F("\twebserver request /"));
      // request->send(200, "text/plain", "Hello, world");

      request->send(LittleFS, "/index.html", "text/html", false, NULL);

      // if (!LittleFS.begin()){
      //   Serial.println(F("An Error has occurred while mounting LittleFS"));
      //   return;
      // }
      
      // File file = LittleFS.open("/test_example.txt", "r");
      // if (!file){
      //   Serial.println(F("Failed to open file for reading"));
      //   return;
      // }
      
      // Serial.println(F("File Content: "));
      // while (file.available()){
      //   Serial.write(file.read());
      // }
      // file.close();
    //   }  
    // else {
    //   ESP_LOGD(TAG," Auth: Failed");
    //   return request->requestAuthentication();
    //   }
  });

  // // Route to load style.css file
  // server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(LittleFS, "/style.css", "text/css");
  // });

  // server->on("/datalog", HTTP_GET, [](AsyncWebServerRequest * request) {
  //   ESP_LOGD(TAG,"Client: %s %s",request->client()->remoteIP().toString().c_str(), request->url().c_str());
  //   if (FlashLogFreeAddress) {
  //     AsyncWebServerResponse *response = request->beginResponse("application/octet-stream", FlashLogFreeAddress, [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
  //       feed_watchdog(); // prevent watchdog resets when downloading large files
  //       return datalog_chunked_read(buffer, maxLen, index);
  //     });
  //     response->addHeader("Content-Disposition", "attachment; filename=datalog");
  //     response->addHeader("Connection", "close");
  //     request->send(response);
  //     }
  //   else {
  //     request->send(400, "text/plain", "ERROR : no data log found");
  //     }
  //   ESP_LOGD(TAG," Datalog Download : Success");
  //   });

  // server->on("/directory", HTTP_GET, [](AsyncWebServerRequest * request)  {
  //   ESP_LOGD(TAG,"Client: %s %s",request->client()->remoteIP().toString(), request->url().c_str());
  //   if (server_authenticate(request)) {
  //     ESP_LOGD(TAG," Auth: Success");
  //     request->send(200, "text/plain", server_directory(true)); // send html formatted table
  //     } 
  //   else {
  //     ESP_LOGD(TAG, " Auth: Failed");
  //     return request->requestAuthentication();
  //     }
  //   });

  // server->on("/file", HTTP_GET, [](AsyncWebServerRequest * request) {
  //   ESP_LOGD(TAG,"Client: %s %s",request->client()->remoteIP().toString().c_str(), request->url().c_str());
  //   if (server_authenticate(request)) {
  //     ESP_LOGD(TAG," Auth: Success");
  //     if (request->hasParam("name") && request->hasParam("action")) {
  //       String fname = "/" + request->getParam("name")->value();
  //       const char* fileName = fname.c_str();
  //       const char *fileAction = request->getParam("action")->value().c_str();
  //       ESP_LOGD(TAG, "Client : %s %s ?name = %s &action = %s",request->client()->remoteIP().toString().c_str(), request->url().c_str(), fileName, fileAction);
  //       if (!LittleFS.exists(fileName)) {
  //         ESP_LOGE(TAG," ERROR: file does not exist");
  //         request->send(400, "text/plain", "ERROR: file does not exist");
  //         } 
  //       else {
  //         ESP_LOGD(TAG," file exists");
  //         if (strcmp(fileAction, "download") == 0) {
  //           ESP_LOGD(TAG, " downloaded");
  //           LittleFSFile = LittleFS.open(fileName, "r");
  //           int sizeBytes = LittleFSFile.size();
  //           AsyncWebServerResponse *response = request->beginResponse("application/octet-stream", sizeBytes, [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
  //             feed_watchdog();
  //             return littlefs_chunked_read(buffer, maxLen);
  //             });
  //           char szBuf[80];
  //           sprintf(szBuf, "attachment; filename=%s", &fileName[1]);// get past the leading '/'
  //           response->addHeader("Content-Disposition", szBuf);
  //           response->addHeader("Connection", "close");
  //           request->send(response);
  //           } 
  //         else if (strcmp(fileAction, "delete") == 0) {
  //           ESP_LOGD(TAG, " deleted");
  //           LittleFS.remove(fileName);
  //           request->send(200, "text/plain", "Deleted File: " + String(fileName));
  //           } 
  //         else {
  //           ESP_LOGE(TAG," ERROR: invalid action param supplied");
  //           request->send(400, "text/plain", "ERROR: invalid action param supplied");
  //           }
  //         }
  //       } 
  //     else {
  //       ESP_LOGE(TAG," ERROR: name and action param required");
  //       request->send(400, "text/plain", "ERROR: name and action params required");
  //       }
  //     } 
  //   else {
  //     ESP_LOGD(TAG," Auth: Failed");
  //     return request->requestAuthentication();
  //     }
  //   });
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


// static void server_handle_upload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
//   if (filename.endsWith(".bin") ) {
//     // .bin files uploaded are processed as application firmware updates
//     server_handle_OTA_update(request, filename, index, data, len, final);
//     }
//   else {
//     // non .bin files are uploaded to the LITTLEFS partition
//     size_t freeBytes = LittleFS.totalBytes() - LittleFS.usedBytes();
//     if (len < freeBytes) {
//       server_handle_littleFS_upload(request, filename, index, data, len, final);
//       }
//     else {
//       ESP_LOGD(TAG, "Cannot upload file size %d bytes, as LittleFS free space = %d bytes", len, freeBytes);
//       request->send(404, "text/plain", "Not enough free space in LittleFS partition");
//       }
//     }
//   }


// // handles non .bin file uploads to the LITTLEFS directory
// static void server_handle_littleFS_upload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
//   // make sure authenticated before allowing upload
//   if (server_authenticate(request)) {
//     ESP_LOGD(TAG,"Client: %s %s",request->client()->remoteIP().toString().c_str(), request->url().c_str());
//     // open the file on first call
//     if (index == 0) {
//       ESP_LOGD(TAG,"Upload Start : %s", filename.c_str());
//       filename = "/" + filename;
//       if (LittleFS.exists(filename)) {
//         bool res = LittleFS.remove(filename);
//         ESP_LOGD(TAG, "Delete file %s : %s", filename, res == false ? "Error" : "OK");
//         }
//       LittleFSFile = LittleFS.open(filename, FILE_WRITE);
//       }

//     if (len) {
//       // stream the incoming chunk to the opened file
//       feed_watchdog();
//       LittleFSFile.write(data, len);
//       ESP_LOGD(TAG,"Writing file : %s, index = %d, len = %d", filename.c_str(), index, len);
//       }

//     if (final) {
//       ESP_LOGD(TAG,"Upload Complete : %s, size = %d", filename.c_str(), index+len);
//       // close the file handle after upload
//       LittleFSFile.close();
//       }
//     } 
//   else {
//     ESP_LOGD(TAG,"Auth: Failed");
//     return request->requestAuthentication();
//     }
//   }


// // handles OTA firmware update
// static void server_handle_OTA_update(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
//   // make sure authenticated before allowing upload
//   if (server_authenticate(request)) {
//     ESP_LOGD(TAG,"Client: %s %s",request->client()->remoteIP().toString().c_str(), request->url().c_str());
//     if (!index) {
//       ESP_LOGD(TAG,"OTA Update Start : %s", filename.c_str());
//       if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
//         Update.printError(Serial);
//         }
//       }

//     if (len) {
//       feed_watchdog();
//       // flashing firmware to ESP
//       if (Update.write(data, len) != len) {
//         Update.printError(Serial);
//         }      
//       ESP_LOGD(TAG,"Writing : %s index = %d len = %d", filename.c_str(), index, len);
//       }

//     if (final) {
//       if (Update.end(true)) { //true to set the size to the current progress
//         ESP_LOGD(TAG,"OTA Complete : %s, size = %d", filename.c_str(), index + len);
//         } 
//       else {
//         Update.printError(Serial);
//         }
//       delayMs(1000);
//       ESP.restart(); // force reboot after firmware update
//       }
//     } 
//   else {
//     ESP_LOGD(TAG,"Auth: Failed");
//     return request->requestAuthentication();
//     }
//   }
// }

web _web;