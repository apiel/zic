#ifndef ZIC_WAVETABLE_FILE_H_
#define ZIC_WAVETABLE_FILE_H_

#if ZIC_USE_LIBSNDFILE == 1
#include "zic_file_audio_libsndfile.h"
#else
#include "zic_file_audio_custom.h"
#endif

#include "zic_wave_base.h"

#define APP_WAVETABLES_MAX 2048

class Zic_Wavetable_File : public Zic_Wave_Base {
protected:
    float sampleIndex = 0.0f;
    float sampleStep = 0.0f;
    uint64_t sampleCount = 0;
    uint64_t start = 0;
    float table[APP_WAVETABLES_MAX];

    float sample()
    {
        if (sampleCount <= 0) {
            return 0.0f;
        }

        // TODO should we use linear interpolation for the wavetable? https://www.youtube.com/watch?v=fufNzqgjej0
        sampleIndex += sampleStep;
        while (sampleIndex >= sampleCount) {
            sampleIndex -= sampleCount;
        }
        return table[(uint16_t)sampleIndex] * amplitude;
    }

    void frequencyUpdated() override
    {
        frequencyUpdated(frequency);
    }

    void frequencyUpdated(float _frequency)
    {
        sampleStep = sampleCount * _frequency / SAMPLE_RATE;
    }

    bool setSkipSample() override
    {
        skipSample = Zic_Wave_Base::setSkipSample() || !audioFile.file;
        return skipSample;
    }

public:
    Zic_File_Audio audioFile;

    Zic_Wavetable_File()
    {
        for (uint64_t i = 0; i < APP_WAVETABLES_MAX && i < sampleCount; i++) {
            table[i] = 0.0f;
        }
        // printf("Zic_Wavetable_File %ld\n", sampleCount);
    }

    float next()
    {
        if (skipSample) {
            return 0;
        }

        return sample() * level;
    }

    float next(float modAmplitude, float modPitch, float modMorph)
    {
        if (skipSample) {
            return 0;
        }

        // Maybe not the way to apply pitch modulation but it does something... :p
        frequencyUpdated(frequency + (modPitch * frequency));

        // modMorph
        // to make modulation on morph, we would have to load the whole wavetable in memory

        return sample() * ((level + modAmplitude) * 0.5f);
    }

    void open(const char* filename)
    {
        audioFile.open(filename);
        sampleCount = (float)audioFile.sampleCount / (float)audioFile.wavetableCount;

        setSkipSample();
        setFrequency(frequency);

        setSampleParams(start, sampleCount);

        // printf("Zic_Wavetable_File open %s %ld\n", filename, sampleCount);
        // printf("open file %s (%p) sample count %ld sample step %f skipSample %d\n", filename, audioFile.file, sampleCount, sampleStep, skipSample);
    }

    void setSampleParams(uint64_t _start, uint64_t _sampleCount)
    {
        // shoud it be: if (_start <= audioFile.sampleCount) {
        // and not care if the target sampleCount is less the requested one
        if (_start + _sampleCount <= audioFile.sampleCount) {
            start = _start;
            sampleCount = _sampleCount;

            audioFile.seekToSample(start);
            for (uint64_t i = 0; i < APP_WAVETABLES_MAX && i < sampleCount; i++) {
                table[i] = audioFile.readSampleFloat();
            }
        }
    }

    void morphPct(float value)
    {
        value = range(value, 0.0f, 1.0f);
        morph(value * (float)audioFile.wavetableCount);
    }

    void morph(float value)
    {
        value = range(value, 0.0f, (float)audioFile.wavetableCount - 1.0f);
        setSampleParams((uint64_t)(value * sampleCount), sampleCount);
    }

    float getMorph()
    {
        return (float)start / (float)sampleCount;
    }
};

#endif
