#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "fileSystemBase.h"
#include "fileSystemLFS.h"

class fileSystem {
  public:
    fileSystem();
    bool setup();
    long totalBytes();
    long usedBytes();
    
    fileSystemLFS instance;
};

extern fileSystem _fileSystem;

extern void setupFileSystem();

#endif