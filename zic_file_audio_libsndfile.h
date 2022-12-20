#ifndef ZIC_FILE_AUDIO_H_
#define ZIC_FILE_AUDIO_H_

#include <sndfile.h>
#include <string.h>

class Zic_File_Audio {
protected:
    Zic_File fileChunks;

    void loadCustomChunks(const char* filename)
    {
        if (fileChunks.open(filename, "rb") == NULL) {
            return;
        }

        uint32_t chunkID;
        uint32_t chunkSize;
        while (fileChunks.read(&chunkID, 4)) {
            // printf("(%d) %c%c%c%c\n", chunkID, (char)(chunkID & 0xFF), (char)((chunkID >> 8) & 0xFF), (char)((chunkID >> 16) & 0xFF), (char)((chunkID >> 24) & 0xFF));
            // Could check that first chunk is RIFF and 3th one is WAVE
            // convert "data" 4 char[] to uint32 in order to compare it to chunkID
            if (chunkID == *(uint32_t*)"data") {
                fileChunks.read(&chunkSize, 4);
                fileChunks.seekFromCurrent(chunkSize);
            } else if (chunkID == *(uint32_t*)"fmt ") {
                fileChunks.read(&chunkSize, 4);
                fileChunks.seekFromCurrent(chunkSize);
            } else if (chunkID == *(uint32_t*)"ZICW") {
                fileChunks.read(&wavetableCount, 4);
                // printf("ZICW Data: %d\n", wavetableCount);
            }
            // See https://github.com/mtytel/vital/blob/main/src/interface/editor_sections/wavetable_edit_section.cpp#L611
            // meta_data.set("clm ", "<!>2048 20000000 wavetable (vital.audio)");
            // vital synth set the chunkID to "clm " and the chunkSize to 2048
            else if (chunkID == *(uint32_t*)"clm ") {
                fileChunks.read(&chunkSize, 4);
                // we could check that string start by "<!>2048"
                // in vital there 256 wavetable of 2048 samples
                wavetableCount = 256;
                // There might be a problem as sample rate is not 44100 but 88200
                // however so far the result is not good
                fileChunks.seekFromCurrent(chunkSize);
            }
        }

        fileChunks.close();
    }

public:
    SF_INFO sfinfo;

    uint64_t sampleCount = 0;
    uint32_t wavetableCount = 1;
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

        loadCustomChunks(filename);

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
