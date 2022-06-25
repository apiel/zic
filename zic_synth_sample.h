#ifndef ZIC_SYNTH_SAMPLE_H_
#define ZIC_SYNTH_SAMPLE_H_

#include "zic_mod_asr.h"
// #include <zic/zic_mod_asrFast.h>
// #include <zic/zic_mod_asrFastQ.h>
#include "zic_mod_filter.h"
#include "zic_wave_sample.h"

/**
 * @brief Basic sample synth
 *
 */
class Zic_Synth_Sample
{
public:
    Zic_Wave_Sample wave;
    Zic_Mod_Asr asr;
    // Zic_Mod_AsrFast asr;
    // Zic_Mod_AsrFastQ asr;
    Zic_Mod_Filter filter;

    Zic_Synth_Sample(Zic_File_ReadAudio* _file) : wave(_file)
    {
    }

    int16_t next()
    {
        return filter.next(asr.next(wave.next()));
    }
};

#endif
