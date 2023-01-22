#ifndef ZIC_OSC_WAVETABLE_H_
#define ZIC_OSC_WAVETABLE_H_

#include "zic_osc_base.h"
#include "zic_wavetable_file.h"

class Zic_Osc_Wavetable : public Zic_Osc_Base {
protected:
    float sampleStep = 0.0f;

    float sample()
    {
        if (wavetable.sampleCount <= 0) {
            return 0.0f;
        }

        // TODO should we use linear interpolation for the wavetable? https://www.youtube.com/watch?v=fufNzqgjej0
        wavetable.sampleIndex += sampleStep;
        while (wavetable.sampleIndex >= wavetable.sampleCount) {
            wavetable.sampleIndex -= wavetable.sampleCount;
        }
        return wavetable.table[(uint16_t)wavetable.sampleIndex] * amplitude;
    }

    void frequencyUpdated() override
    {
        frequencyUpdated(frequency);
    }

    void frequencyUpdated(float _frequency)
    {
        sampleStep = wavetable.sampleCount * _frequency / SAMPLE_RATE;
    }

    bool setSkipSample() override
    {
        skipSample = Zic_Osc_Base::setSkipSample() || !wavetable.audioFile.file;
        return skipSample;
    }

public:
    Zic_Wavetable_File wavetable;

    Zic_Osc_Wavetable()
    {
        setFrequency(frequency);
    }

    void open(const char* filename)
    {
        wavetable.open(filename);
        setSkipSample();
        setFrequency(frequency);
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
};

#endif
