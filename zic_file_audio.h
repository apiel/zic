#ifndef ZIC_FILE_AUDIO_H_
#define ZIC_FILE_AUDIO_H_

#include "zic_file.h"

#define BUFF_SIZE 6720

class Zic_File_Audio : public Zic_File {
protected:
    typedef struct WavHeader {
        uint32_t ChunkID;
        uint32_t ChunkSize;
        uint32_t Format;
        uint32_t Subchunk1ID;
        uint32_t Subchunk1Size;
        uint16_t AudioFormat;
        uint16_t NumChannels;
        uint32_t SampleRate;
        uint32_t ByteRate;
        uint16_t BlockAlign;
        uint16_t BitsPerSample;
        uint32_t Subchunk2ID;
        uint32_t Subchunk2Size;
    } WavHeader;

    WavHeader header;

public:
    uint64_t start = 0;
    uint64_t end = 0;
    uint64_t sampleCount = 0;

    Zic_File_Audio()
    {
    }

    Zic_File_Audio(const char* filename)
    {
        open(filename);
    }

    void* open(const char* filename)
    {
        if (Zic_File::open(filename, "rb") == NULL) {
            return NULL;
        }

        Zic_File::read((uint8_t*)&header, sizeof(WavHeader));

        // skip over any other chunks before the "data" chunk
        bool additionalHeaderDataPresent = false;
        while (header.Subchunk2ID != 1635017060) {
            seekFromCurrent(4);
            Zic_File::read((uint8_t*)&header.Subchunk2ID, 4);
            additionalHeaderDataPresent = true;
        }

        if (additionalHeaderDataPresent) {
            // read the value of Subchunk2Size, the one populated when reading 'WavHeader' structure is wrong
            Zic_File::read((uint8_t*)&header.Subchunk2Size, 4);
        }

        // Should we do this?
        // if (header.BitsPerSample != 16 || header.AudioFormat != 1) {
        //     file = NULL;
        //     return NULL;
        // }

        start = tell();
        seekFromEnd(0);
        end = tell();
        restart();
        sampleCount = (end - start) / (header.BitsPerSample); // * header.NumChannels

        // printf("Audio file %d %d %d start %ld end %ld sampleCount %ld\n",
        //     header.BitsPerSample, header.AudioFormat, header.NumChannels, (long)start, (long)end, (long)sampleCount);

        return file;
    }

    void seekToSample(uint64_t pos)
    {
        seekFromStart(start + (pos * header.BitsPerSample)); // * header.NumChannels
    }

    void restart()
    {
        seekFromStart(start);
    }
};

#endif
