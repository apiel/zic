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
    uint64_t bitsCount = 0;
    uint64_t sampleCount = 0;
    uint16_t wavetableCount = 1;
    uint8_t bytesPerSample = sizeof(int16_t);

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
                Zic_File::read(&bitsCount, 4);
                start = tell();
                end = start + bitsCount;
                seekFromCurrent(bitsCount);
            } else if (chunkID == 544501094) { // fmt
                Zic_File::read(&chunkSize, 4);
                if (Zic_File::read((uint8_t*)&header, sizeof(WavHeader)) != chunkSize) {
                    // printf("Something went wrong reading the fmt chunk\n");
                    // return NULL; // we cann still continue
                }
            } else if (chunkID == 1464027482) { // ZICW
                Zic_File::read(&wavetableCount, 4);
                // printf("ZICW Data: %d\n", wavetableCount);
            }
            // } else if (chunkID == 1414349641) { // ICMT
            // #define AUDIO_FILE_DATA_SIZE 128
            //     char data[AUDIO_FILE_DATA_SIZE];
            //     Zic_File::read(&chunkSize, 4);
            //     Zic_File::read(&data, chunkSize > AUDIO_FILE_DATA_SIZE ? AUDIO_FILE_DATA_SIZE : chunkSize);
            //     printf("ICMT Data: %s\n\n", data);
            // }
        }

        restart();

        if (header.BitsPerSample != 16) {
            // printf("Only 16 bit WAV files are supported\n");
            return NULL;
        }
        // NOTE is it even necessary to get the sampleCount
        // or at least to make the wavetable calculation using the sampleCount
        // instead of the bitsCount? We could just use the bitsCount, and avoid
        // to multiply by bytesPerSample
        sampleCount = bitsCount / bytesPerSample; // * header.NumChannels

        // printf("Audio file %s bitPerSample %d  format %d chan %d rate %d start %ld end %ld sampleCount %ld\n", filename,
        //     header.BitsPerSample, header.AudioFormat, header.NumChannels, header.SampleRate,
        //     (long)start, (long)end, (long)sampleCount);

        return end ? file : NULL;
    }

    void seekToSample(uint64_t pos)
    {
        seekFromStart(start + (pos * bytesPerSample));
    }

    void restart()
    {
        seekFromStart(start);
    }
};

#endif
