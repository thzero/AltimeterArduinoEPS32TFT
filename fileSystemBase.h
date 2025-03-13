#ifndef _FILESYSTEM_BASE_H
#define _FILESYSTEM_BASE_H

#include <ArduinoJson.h>

class fileSystemBase {
  public:
    virtual void loadConfigSim(JsonArray configs);
    bool setup();
    long totalBytes();
    long usedBytes();
};

#endif