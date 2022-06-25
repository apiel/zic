#ifndef ZIC_FILE_H_
#define ZIC_FILE_H_

#include <stdint.h>

class Zic_File {
public:
    void* file = NULL;

    Zic_File()
    {
    }

    Zic_File(const char* filename, const char* mode)
    {
        open(filename, mode);
    }

    ~Zic_File()
    {
        close();
    }

    void* open(const char* filename, const char* mode)
    {
        return NULL;
    }

    bool read(void* ptr, uint16_t size)
    {
        return false;
    }

    uint64_t tell()
    {
        return 0;
    }

    bool seekFromStart(uint64_t offset)
    {
        return false;
    }

    bool seekFromCurrent(uint64_t offset)
    {
        return false;
    }

    bool close()
    {
        return false;
    }
};

#endif
