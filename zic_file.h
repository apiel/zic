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

    virtual bool isOpen()
    {
        return file != NULL;
    }

    virtual void* open(const char* filename, const char* mode)
    {
        return NULL;
    }

    virtual uint64_t read(void* ptr, uint16_t size)
    {
        return 0;
    }

    virtual uint64_t write(void* ptr, uint16_t size)
    {
        return 0;
    }

    virtual uint64_t tell()
    {
        return 0;
    }

    virtual bool seekFromStart(uint64_t offset)
    {
        return false;
    }

    virtual bool seekFromCurrent(uint64_t offset)
    {
        return false;
    }

    virtual bool seekFromEnd(uint64_t offset)
    {
        return false;
    }

    virtual bool close()
    {
        return false;
    }
};

#endif
