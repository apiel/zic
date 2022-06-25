#ifndef ZIC_FILE_READ_AUDIO_H_
#define ZIC_FILE_READ_AUDIO_H_

#include <stdint.h>

class Zic_File_ReadAudio {
public:
    uint64_t length;
    virtual bool open(char* filename) = 0;
    virtual bool read(void* ptr, uint16_t size) = 0;
    virtual bool seek(uint64_t offset) = 0;
    virtual bool close() = 0;
};

#endif