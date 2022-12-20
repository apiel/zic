#ifndef ZIC_FILE_AUDIO_H_
#define ZIC_FILE_AUDIO_H_

#include <sndfile.h>
#include <string.h>

class Zic_File_Audio {
public:
    SF_INFO sfinfo;

    uint64_t sampleCount = 0;
    // FIXME
    // uint32_t wavetableCount = 1;
    uint32_t wavetableCount = 64;
    // uint8_t bytesPerSample = sizeof(float);
    SNDFILE* file = NULL;

    Zic_File_Audio()
    {
        memset(&sfinfo, 0, sizeof(sfinfo));
    }

    ~Zic_File_Audio()
    {
        close();
    }

    void close()
    {
        if (file) {
            sf_close(file);
        }
    }

    void* open(const char* filename)
    {
        close();

        if (!(file = sf_open(filename, SFM_READ, &sfinfo))) {
            printf("Error: could not open file %s\n", filename);
            return NULL;
        }
        sampleCount = sfinfo.frames;
        // printf("Audio file %s sampleCount %ld\n", filename, (long)sampleCount);
        return file;
    }

    float readSampleFloat()
    {
        float sample;
        sf_read_float(file, &sample, 1);
        return sample;
    }

    int16_t readSampleInt16()
    {
        int16_t sample;
        sf_read_short(file, &sample, 1);
        return sample;
    }

    void seekToSample(uint64_t pos)
    {
        sf_seek(file, pos, SEEK_SET);
    }

    void restart()
    {
        sf_seek(file, 0, SEEK_SET);
    }
};

#endif
