#ifndef __FS_MANAGER_H__
#define __FS_MANAGER_H__

#include <stdint.h>

class FsManager 
{

public:
    bool setup();

public:
    bool SaveToFs(const uint8_t *payload, unsigned int length);
    unsigned int ReadFromFs(uint8_t *buffer, unsigned int length);
};

#endif // __FS_MANAGER_H__