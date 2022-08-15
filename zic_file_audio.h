#ifndef ZIC_FILE_AUDIO_H_
#define ZIC_FILE_AUDIO_H_

#include "zic_file.h"

class Zic_File_Audio : public Zic_File {
protected:
    typedef struct WavHeader {
        uint16_t AudioFormat;
        uint16_t NumChannels;
        uint32_t SampleRate;
        uint32_t ByteRate;
        uint16_t BlockAlign;
        uint16_t BitsPerSample;
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

        bool dataAvailable = false;
        uint32_t chunkID;
        uint32_t chunkSize;
        while (Zic_File::read(&chunkID, 4)) {
            // printf("(%d) %c%c%c%c\n", chunkID, (char)(chunkID & 0xFF), (char)((chunkID >> 8) & 0xFF), (char)((chunkID >> 16) & 0xFF), (char)((chunkID >> 24) & 0xFF));
            // 1414744396 -> LIST
            // 70468681 -> ID3
            // 1179011410 -> RIFF
            // 1163280727 -> WAVE
            // 544501094 -> fmt
            // Could check that first chunk is RIFF and 3th one is WAVE
            if (chunkID == 1635017060) { // data
                Zic_File::read(&chunkSize, 4);
                start = tell();
                end = start + chunkSize;
                seekFromCurrent(chunkSize);
                dataAvailable = true;
            } else if (chunkID == 544501094) { // fmt
                Zic_File::read(&chunkSize, 4);
                if (Zic_File::read((uint8_t*)&header, sizeof(WavHeader)) != chunkSize) {
                    // printf("Something went wrong reading the fmt chunk\n");
                    // return NULL; // we cann still continue
                }
            } else if (chunkID == 1464027482) { // ZICW
                Zic_File::read(&chunkSize, 4);
                printf("ZICW Data: %d\n", chunkSize);
                // printf("(%d) %c%c%c%c hex: x%X x%X x%X x%X\n", chunkSize,
                //     (char)(chunkSize & 0xFF), (char)((chunkSize >> 8) & 0xFF), (char)((chunkSize >> 16) & 0xFF), (char)((chunkSize >> 24) & 0xFF),
                //     (chunkSize & 0xFF), ((chunkSize >> 8) & 0xFF), ((chunkSize >> 16) & 0xFF), ((chunkSize >> 24) & 0xFF));
                // printf("\n");
            }
            // } else if (chunkID == 1414349641) { // ICMT
            // #define AUDIO_FILE_DATA_SIZE 128
            //     char data[AUDIO_FILE_DATA_SIZE];
            //     Zic_File::read(&chunkSize, 4);
            //     Zic_File::read(&data, chunkSize > AUDIO_FILE_DATA_SIZE ? AUDIO_FILE_DATA_SIZE : chunkSize);
            //     printf("ICMT Data: %s\n\n", data);
            // }
        }

        // printf("Last chunk(%d) %c%c%c%c\n", chunkID, (char)(chunkID & 0xFF), (char)((chunkID >> 8) & 0xFF), (char)((chunkID >> 16) & 0xFF), (char)((chunkID >> 24) & 0xFF));

        restart();
        // this is false, it should be (end - start) / (header.BitsPerSample / 8)
        // as 16 bits is made of 2 bytes (of 8 bits)
        sampleCount = (end - start) / (header.BitsPerSample); // * header.NumChannels

        // printf("%s: %d channels, %d Hz, %d bits, chunksize %d Subchunk1Size %d DatachunkSize %d\n",
        //     filename, header.NumChannels, header.SampleRate, header.BitsPerSample, header.chunkSize, header.Subchunk1Size, header.DatachunkSize);

        // printf("Audio file %d %d %d start %ld end %ld sampleCount %ld\n",
        //     header.BitsPerSample, header.AudioFormat, header.NumChannels, (long)start, (long)end, (long)sampleCount);

        return dataAvailable ? file : NULL;
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
