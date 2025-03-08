#ifndef _FILESYSTEM_BASE_H
#define _FILESYSTEM_BASE_H

class fileSystemBase {
  public:
    bool setup();
    long totalBytes();
    long usedBytes();
};

#endif