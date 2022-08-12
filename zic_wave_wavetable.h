#ifndef ZIC_WAVE_WAVETABLE_H_
#define ZIC_WAVE_WAVETABLE_H_

#include "zic_wave_base.h"
#include "zic_wavetable_base.h"

// TODO coundlt we just cache the incrementation step/rate per frequency
// as we most likely always use same frequencies base on musical notes...
// and then always increment with this step

// TODO use linear interpolation for the wavetable?

class Zic_Wave_Wavetable : public Zic_Wave_Base
{
protected:
    // FIXME still out of tune
    int16_t sample(uint32_t *_freq)
    {
        int i = (FREQ_PI * (*_freq) * time + phase) * sampleCount;
        // return table[i % sampleCount];
        // FIXME modulo, dont use motulo but comparison
        return table[((i % sampleCount) + pos) % size];
    }

public:
    int16_t *table = NULL;
    uint16_t sampleCount = 0;
    uint16_t size = 0;
    uint16_t pos = 0;

    Zic_Wave_Wavetable()
    {
        // This might be dangerous!!
    }

    Zic_Wave_Wavetable(Zic_Wavetable_Base *wavetable)
    {
        set(wavetable);
    }

    void set(Zic_Wavetable_Base *wavetable)
    {
        table = wavetable->table;
        size = wavetable->size;
        sampleCount = wavetable->sampleCount;
    }
};

#endif
