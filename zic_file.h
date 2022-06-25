#ifndef ZIC_FILE_H_
#define ZIC_FILE_H_

#include <stdint.h>

// TODO should this be a class?

void* zic_file_open(const char* filename, const char* mode)
{
    return NULL;
}

bool zic_file_read(void* file, void* ptr, uint16_t size)
{
    return false;
}

bool zic_file_seek(void* file, uint64_t offset)
{
    return false;
}

bool zic_file_close(void* file)
{
    return false;
}

#endif
