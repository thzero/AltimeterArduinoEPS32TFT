#ifndef _FILESYSTEM_LFS_H
#define _FILESYSTEM_LFS_H

#include "fileSystemBase.h"

class fileSystemLFS: public fileSystemBase {
  public:
    fileSystemLFS();
    bool setup();
    long totalBytes();
    long usedBytes();
};

#endif