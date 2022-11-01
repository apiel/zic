#ifndef ZIC_EFFECT_FILTER_H_
#define ZIC_EFFECT_FILTER_H_

#include <math.h>

#include "./zic_def.h"

// https://www.martin-finke.de/articles/audio-plugins-013-filter/
// https://www.musicdsp.org/en/latest/Filters/29-resonant-filter.html

class Zic_Effect_Filter {
protected:
    int16_t buf0 = 0.0;
    int16_t buf1 = 0.0;
    int16_t buf2 = 0.0;
    int16_t buf3 = 0.0;

    float feedback;
    void calculateVar()
    {
        // cutoff = 2.0*sin(pi*freq/samplerate);
        feedback = resonance + resonance / (1.0 - cutoff);
        // Serial.printf("cutoff %.2f calc %.2f res %.2f feedback %.2f\n", cutoff, cutoff, resonance, feedback);
    }

public:
    enum FilterMode {
        FILTER_MODE_OFF,
        FILTER_MODE_LOWPASS_12,
        FILTER_MODE_LOWPASS_24,
        FILTER_MODE_HIGHPASS_12,
        FILTER_MODE_HIGHPASS_24,
        FILTER_MODE_BANDPASS_12,
        FILTER_MODE_BANDPASS_24,
        FILTER_MODE_COUNT,
    };

    // uint16_t frequency = (uint16_t)(SAMPLE_RATE / 2.0);
    uint16_t frequency = 8000;
    float cutoff = 0.99;
    float resonance = 0.0;
    uint8_t mode = FILTER_MODE_LOWPASS_24;

    Zic_Effect_Filter()
    {
        // calculateVar();
        setFrequency(frequency);
    };

    int16_t next(int16_t inputValue)
    {
        if (mode == FILTER_MODE_OFF || inputValue == 0) {
            return inputValue;
        }
        buf0 += cutoff * (inputValue - buf0 + feedback * (buf0 - buf1));
        buf1 += cutoff * (buf0 - buf1);
        buf2 += cutoff * (buf1 - buf2);
        buf3 += cutoff * (buf2 - buf3);
        switch (mode) {
        case FILTER_MODE_LOWPASS_12:
            return buf1;
        case FILTER_MODE_LOWPASS_24:
            return buf3;
        case FILTER_MODE_HIGHPASS_12:
            return inputValue - buf0;
        case FILTER_MODE_HIGHPASS_24:
            return inputValue - buf3;
        case FILTER_MODE_BANDPASS_12:
            return buf0 - buf1;
        case FILTER_MODE_BANDPASS_24:
            return buf0 - buf3;
        default:
            return 0.0;
        }
    }

    void setFrequency(uint16_t freq)
    {
        frequency = range(freq, 0, 8000);
        cutoff = 2.0 * sin(M_PI * frequency / SAMPLE_RATE);
        // printf("cutoff %.2f\n", cutoff);
        set(cutoff, resonance);
    }

    // could set cutoff for modulation
    // but maybe it would just be better to precalculate all the possible value for the frequency...
    //
    // or use 0 to .99 to set the value and use the getFrequencyFromCutoff ???
    void setCutoff(float _cutoff)
    {
        set(_cutoff, resonance);
    };

    float getFrequencyFromCutoff()
    {
        return SAMPLE_RATE * asin(cutoff / 2.0) / M_PI;
    }

    void setResonance(float _res)
    {
        set(cutoff, _res);
    };

    void set(float _cutoff, float _res)
    {
        // cutoff cannot be 1.0 else div by zero
        cutoff = range(_cutoff, 0.01, 0.99);
        resonance = range(_res, 0.00, 0.99);
        calculateVar();
    }

    void setFilterMode(int8_t value)
    {
        mode = value % FILTER_MODE_COUNT;
    }

    const char* getName()
    {
        switch (mode) {
        case FILTER_MODE_LOWPASS_12:
            return "LPF 12dB";
        case FILTER_MODE_LOWPASS_24:
            return "LPF 24dB";
        case FILTER_MODE_HIGHPASS_12:
            return "HPF 12dB";
        case FILTER_MODE_HIGHPASS_24:
            return "HPF 24dB";
        case FILTER_MODE_BANDPASS_12:
            return "BPF 12dB";
        case FILTER_MODE_BANDPASS_24:
            return "BPF 24dB";
        default:
            return "OFF";
        }
    }
};

#endif
