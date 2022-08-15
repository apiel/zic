#ifndef ZIC_WAVE_OSC_H_
#define ZIC_WAVE_OSC_H_

#include <math.h>
#include <stdint.h>

#include "../fastTrigo.h"
#include "zic_wave_base.h"

// To do use less floats, let round Freq to 100 up and then divide by 100 PI
#ifndef FREQ_MULT
#define FREQ_MULT 100
#endif

#ifndef FREQ_PI
#define FREQ_PI M_PI / FREQ_MULT
#endif

enum {
    OSC_SINE,
    OSC_SQUARE,
    OSC_TRIANGLE,
    OSC_SAW,
    OSC_NOIZE,
    OSC_COUNT
};

const char* getOscName(uint8_t _oscType)
{
    switch (_oscType) {
    case OSC_SINE:
        return "Sine";
    case OSC_SQUARE:
        return "Square";
    case OSC_TRIANGLE:
        return "Triangle";
    case OSC_SAW:
        return "Saw";
    case OSC_NOIZE:
        return "Noize";
    }
    return "Unknown";
}

// FIXME switch to int16 instead of double and float
// however, i would argue to use wavetable in general
// unless those wave can be edited

// https://github.com/audiowaves/simpleWavesGenerator
// https://olehc.medium.com/basic-sound-waves-with-c-and-juce-50ec9f0bfe5c
class Zic_Wave_Osc : public Zic_Wave_Base {
protected:
    uint32_t freq = frequency * FREQ_MULT;

    void frequencyUpdated() override
    {
        freq = frequency * FREQ_MULT;
    }

    double sine()
    {
        // This use too much ressources
        // return sin(M_PI_2 * freq * time + phase);

        return fastSine(M_PI_2 * freq * time + phase);
    }

    double square()
    {
        double fullPeriodTime = 1.0 / freq;
        double halfPeriodTime = fullPeriodTime * 0.5;
        double localTime = fmod(time, fullPeriodTime);
        return localTime < halfPeriodTime ? 1 : -1;
    }

    double triangle()
    {
        double fullPeriodTime = 1.0 / freq;
        double localTime = fmod(time, fullPeriodTime);

        double value = localTime / fullPeriodTime;

        if (value < 0.25) {
            return value * 4;
        }
        if (value < 0.75) {
            return 2.0 - (value * 4.0);
        }
        return value * 4 - 4.0;
    }

    double saw()
    {
        double fullPeriodTime = 1.0 / freq;
        double localTime = fmod(time, fullPeriodTime);

        return ((localTime / fullPeriodTime) * 2 - 1.0);
    }

    double noize()
    {
        return (rand() % 10000) * 0.00001;
    }

    double sample()
    {
        time += DELTA_TIME;

        switch (oscType) {
        case OSC_SINE:
            return sine();
        case OSC_SQUARE:
            return square();
        case OSC_TRIANGLE:
            return triangle();
        case OSC_SAW:
            return saw();
        case OSC_NOIZE:
            return noize();
        }
        return 0;
    }

public:
    // uint8_t oscType = OSC_TRIANGLE;
    uint8_t oscType = OSC_SINE;

    void reset()
    {
        time = 0.0;
    }
};

#endif
