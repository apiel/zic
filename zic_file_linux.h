// Keep this ifndef naming to get priority over zic_file.h.
#ifndef ZIC_FILE_H_
#define ZIC_FILE_H_

#include <stdint.h>
#include <stdio.h>

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

    virtual void* open(const char* filename, const char* mode)
    {
        close();
        file = fopen(filename, mode);
        return file;
    }

    virtual bool isOpen()
    {
        return file != NULL;
    }

    virtual uint64_t read(void* ptr, uint16_t size)
    {
        return (uint64_t)fread(ptr, size, 1, (FILE*)file);
    }

    virtual uint64_t write(void* ptr, uint16_t size)
    {
        return (uint64_t)fwrite(ptr, size, 1, (FILE*)file);
    }

    virtual bool seekFromStart(uint64_t offset)
    {
        return fseek((FILE*)file, offset, SEEK_SET) != -1;
    }

    virtual bool seekFromCurrent(uint64_t offset)
    {
        return fseek((FILE*)file, offset, SEEK_CUR) != -1;
    }

    virtual bool seekFromEnd(uint64_t offset)
    {
        return fseek((FILE*)file, offset, SEEK_END) != -1;
    }

    virtual uint64_t tell()
    {
        return ftell((FILE*)file);
    }

    virtual bool close()
    {
        if (file == NULL) {
            return true;
        }
        if (fclose((FILE*)file) == 0) {
            file = NULL;
            return true;
        }
        return false;
    }
};

#endif
