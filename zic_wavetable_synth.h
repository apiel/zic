#ifndef ZIC_WAVETABLE_SYNTH_H_
#define ZIC_WAVETABLE_SYNTH_H_

#include "zic_mod_asr.h"
// #include <zic/zic_mod_asrFast.h>
// #include <zic/zic_mod_asrFastQ.h>
#include "zic_mod_filter.h"
#include "zic_wave_wavetable.h"

/**
 * @brief Basic wavetable synth combining wave, modulation, effect...
 *
 */
class Zic_Wavetable_Synth
{
public:
    Zic_Wave_Wavetable wave;
    Zic_Mod_Asr asr;
    // Zic_Mod_AsrFast asr;
    // Zic_Mod_AsrFastQ asr;
    Zic_Mod_Filter filter;

    Zic_Wavetable_Synth(Zic_Wavetable_Base *wavetable) : wave(wavetable)
    {
    }

    int16_t next()
    {
        return filter.next(asr.next(wave.next()));
    }
};

#endif
