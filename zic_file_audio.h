#ifndef ZIC_FILE_AUDIO_H_
#define ZIC_FILE_AUDIO_H_

#include "zic_file.h"

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

    Zic_File_Audio()
    {
    }

    Zic_File_Audio(const char* filename)
    {
        open(filename);
    }

    void* open(const char* filename)
    {
        Zic_File::open(filename, "rb");
        read((uint8_t*)&header, sizeof(WavHeader));

        // skip over any other chunks before the "data" chunk
        bool additionalHeaderDataPresent = false;
        while (header.Subchunk2ID != 1635017060) {
            seekFromCurrent(4);
            read((uint8_t*)&header.Subchunk2ID, 4);
            additionalHeaderDataPresent = true;
        }

        if (additionalHeaderDataPresent) {
            // read the value of Subchunk2Size, the one populated when reading 'WavHeader' structure is wrong
            read((uint8_t*)&header.Subchunk2Size, 4);
        }

        start = tell();

        printf("Audio file %d %d start %ld\n", header.BitsPerSample, header.AudioFormat, start);

        return file;
    }

    void restart()
    {
        seekFromStart(start);
    }
};

#endif
