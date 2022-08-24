#ifndef ZIC_FILE_WAV_H_
#define ZIC_FILE_WAV_H_

#include "zic_file.h"

class Zic_File_Wav : public Zic_File {
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

    void* loadWav() {

        uint32_t chunkID;
        uint32_t chunkSize;
        while (Zic_File::read(&chunkID, 4)) {
            // printf("(%d) %c%c%c%c\n", chunkID, (char)(chunkID & 0xFF), (char)((chunkID >> 8) & 0xFF), (char)((chunkID >> 16) & 0xFF), (char)((chunkID >> 24) & 0xFF));
            // Could check that first chunk is RIFF and 3th one is WAVE
            // convert "data" 4 char[] to uint32 in order to compare it to chunkID
            if (chunkID == *(uint32_t*)"data") {
                Zic_File::read(&audioDataCount, 4);
                audioDataStart = tell();
                seekFromCurrent(audioDataCount);
            } else if (chunkID == *(uint32_t*)"fmt ") {
                Zic_File::read(&chunkSize, 4);
                if (Zic_File::read((uint8_t*)&header, sizeof(WavHeader)) != chunkSize) {
                    // printf("Something went wrong reading the fmt chunk\n");
                    // return NULL; // we cann still continue
                }
            } else if (chunkID == *(uint32_t*)"ZICW") {
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
        sampleCount = audioDataCount / bytesPerSample; // * header.NumChannels

        // printf("Audio file %s bitPerSample %d  format %d chan %d rate %d start %ld sampleCount %ld\n", filename,
        //     header.BitsPerSample, header.AudioFormat, header.NumChannels, header.SampleRate,
        //     (long)audioDataStart, (long)sampleCount);

        return audioDataStart ? file : NULL;
    }

public:
    uint64_t audioDataStart = 0;
    uint64_t audioDataCount = 0;
    uint64_t sampleCount = 0;
    uint32_t wavetableCount = 1;
    uint8_t bytesPerSample = sizeof(int16_t);

    Zic_File_Wav()
    {
    }

    Zic_File_Wav(const char* filename)
    {
        open(filename);
    }

    void* open(const char* filename)
    {
        if (Zic_File::open(filename, "rb") == NULL) {
            return NULL;
        }
        return loadWav();
    }

    /**
     * @brief seek to 16bits sample
     *
     * in 16bits per sample, 2 bytes are used to store 1 sample
     * therefor we multiply the position by 2 (bytesPerSample)
     * and seek from the start of the audio data
     *
     * @param pos
     */
    void seekToSample(uint64_t pos)
    {
        seekFromStart(audioDataStart + (pos * bytesPerSample));
    }

    void restart()
    {
        seekFromStart(audioDataStart);
    }
};

#endif
