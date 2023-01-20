#ifndef ZIC_OSC_WAVETABLE_H_
#define ZIC_OSC_WAVETABLE_H_

#include "zic_wavetable_file.h"

class Zic_Osc_Wavetable: public Zic_Wavetable_File {
protected:
    float sampleStep = 0.0f;

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
};

#endif
