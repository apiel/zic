#ifndef ZIC_WAVE_FILE_H_
#define ZIC_WAVE_FILE_H_

// #include "zic_file_audio.h"
#include "zic_file_audio2.h"
#include "zic_wave_base.h"

class Zic_Wave_File : public Zic_Wave_Base {
protected:
    float sampleIndex = 0.0f;
    float sampleStep = 0.0f;
    uint64_t sampleCount = 0;
    uint64_t start = 0;

    float sample()
    {
        if (isWavetable) {
            // TODO should we use linear interpolation for the wavetable? https://www.youtube.com/watch?v=fufNzqgjej0
            sampleIndex += sampleStep;
            while (sampleIndex >= sampleCount) {
                sampleIndex -= sampleCount;
            }
            audioFile.seekToSample((uint64_t)sampleIndex + start);
        }

        return audioFile.readSampleFloat() * amplitude;
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
        sampleCount = isWavetable && audioFile.wavetableCount > 1
            ? (float)audioFile.sampleCount / (float)audioFile.wavetableCount
            : audioFile.sampleCount;

        setSkipSample();
        setFrequency(frequency);

        // printf("open file %s (%p) sample count %ld sample step %f skipSample %d\n", filename, audioFile.file, sampleCount, sampleStep, skipSample);
    }

    void reset()
    {
        if (!isWavetable) {
            audioFile.restart();
        }
    }

    void setSampleParams(uint64_t _start, uint64_t _sampleCount)
    {
        if (_start + _sampleCount <= audioFile.sampleCount) {
            start = _start;
            sampleCount = _sampleCount;
        }
    }

    void morph(float value)
    {
        value = range(value, 0.0f, (float)audioFile.wavetableCount - 1.0f);
        if (isWavetable) {
            setSampleParams((uint64_t)(value * sampleCount), sampleCount);
        }
    }

    float getMorph()
    {
        if (isWavetable) {
            return (float)start / (float)sampleCount;
        }
        return 0.0f;
    }
};

#endif
