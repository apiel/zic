#ifndef ZIC_WAVE_FILE_H_
#define ZIC_WAVE_FILE_H_

#include "zic_file_audio.h"
#include "zic_wave_base.h"

class Zic_Wave_File : public Zic_Wave_Base {
protected:
    float sampleIndex = 0.0f;
    float sampleStep = 0.0f;
    uint64_t sampleCount = 0;

    int16_t sample()
    {
        int16_t bit = 0;
        if (isWavetable) {
            // TODO should we use linear interpolation for the wavetable? https://www.youtube.com/watch?v=fufNzqgjej0
            sampleIndex += sampleStep;
            while (sampleIndex >= sampleCount) {
                sampleIndex -= sampleCount;
            }
            audioFile.seekToSample((uint64_t)sampleIndex);
        }
        audioFile.read(&bit, sizeof(bit));

        return bit * amplitude / 100;
    }

    void frequencyUpdated() override
    {
        // could even cache frequency / SAMPLE_RATE or even the whole calc for each frequency
        // however, this happen only once every noteOn, it is not that much that it would require
        // to cache it
        sampleStep = sampleCount * frequency / SAMPLE_RATE;
    }

    bool setSkipSample() override
    {
        skipSample = Zic_Wave_Base::setSkipSample() || !audioFile.file;
        return skipSample;
    }

public:
    Zic_File_Audio audioFile;

    bool isWavetable = false;

    void open(const char* filename, bool _isWavetable = false)
    {
        audioFile.open(filename);
        isWavetable = _isWavetable;
        sampleCount = audioFile.sampleCount;
        setSkipSample();
    }

    void reset()
    {
        if (!isWavetable) {
            audioFile.restart();
        }
    }
};

#endif
