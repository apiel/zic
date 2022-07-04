#ifndef ZIC_SYNTH_FILE_H_
#define ZIC_SYNTH_FILE_H_

#include "zic_mod_asr.h"
// #include <zic/zic_mod_asrFast.h>
// #include <zic/zic_mod_asrFastQ.h>
#include "zic_mod_filter.h"
#include "zic_wave_file.h"

/**
 * @brief Basic sample synth
 *
 */
class Zic_Synth_File
{
public:
    Zic_Wave_File wave;
    Zic_Mod_Asr asr;
    // Zic_Mod_AsrFast asr;
    // Zic_Mod_AsrFastQ asr;
    Zic_Mod_Filter filter;

    // Zic_Synth_File()
    // {
    // }

    int16_t next()
    {
        return filter.next(asr.next(wave.next()));
    }
};

#endif
