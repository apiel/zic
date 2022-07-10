#ifndef ZIC_WAVE_BASE_H_
#define ZIC_WAVE_BASE_H_

#include <math.h>

#include "zic_def.h"
#include "zic_note.h"

#ifndef FREQ_MULT
#define FREQ_MULT 100
#endif

#ifndef FREQ_DIV
#define FREQ_DIV 1.0f / FREQ_MULT
#endif

#ifndef FREQ_PI
#define FREQ_PI M_PI / FREQ_MULT
#endif

#define RESET_TIME DELTA_TIME * 360 * 10000

class Zic_Wave_Base
{
protected:
    float time = 0.0;

    virtual int16_t sample(uint32_t *freq) { return 0; }
    uint32_t frequency = 103.82617439443122f * FREQ_MULT; // C3

    // Pre-calculation
    uint32_t freq = frequency; // * pitch;

    int8_t amplitude = 100;
    // float pitch = 1.0f;

    uint16_t phase = 0; // 0 to 360

    // TODO
    // see https://teropa.info/blog/2016/08/10/frequency-and-pitch.html
    void updateFreq()
    {
        freq = frequency; // and do something with pitch
    }

public:
    // TODO dont use float for that
    // float pitchMod = 1.0f;

    int16_t next()
    {
        time += DELTA_TIME;

        // FIXME trying to fix changing tone, maybe this would work...
        if (time >= RESET_TIME) {
            // SDL_Log("Reset timing %.6f", time);
            time = 0.0;
        }

        // use bitwise >> 8 to reduce amplitude (division by 256)
        // we could have a higher quality wavetable to int32 using a higher bitwise value
        // but is it really necessary? int16 make a gain on firmware size!
        return (amplitude * sample(&freq)) >> 8;
    }

    /**
     * @brief Set the Phase of the wave
     *
     * @param value between 0° and 360°
     */
    void setPhase(uint16_t value)
    {
        phase = range(value, 0, 360);
    }

    /**
     * @brief Get the Phase of the wave
     *
     * @return uint16_t
     */
    uint16_t getPhase()
    {
        return phase;
    }

    /**
     * @brief Set the Amplitude of the wave
     *
     * @param value between 0 and 100
     */
    void setAmplitude(int8_t value)
    {
        amplitude = range(value, 0, 100);
    }

    /**
     * @brief Get the Amplitude of the wave
     *
     * @return uint16_t
     */
    uint16_t getAmplitude()
    {
        return amplitude;
    }

    /**
     * @brief Set the Frequency using note.
     *
     * @param value
     */
    virtual void setNote(uint8_t note)
    {
        setFrequency(Zic::NOTE_FREQ[note]);
    }

    /**
     * @brief Set the Frequency of the wave. Use zic_note.h to assign notes.
     *
     * @param value
     */
    void setFrequency(float value)
    {
        frequency = value * FREQ_MULT;
        updateFreq();
    }

    /**
     * @brief Get the Frequency of the wave
     *
     * @return float
     */
    float getFrequency()
    {
        return frequency * FREQ_DIV;
    }

    // /**
    //  * @brief Set the Pitch of the wave
    //  *
    //  * @param value
    //  */
    // void setPitch(float value)
    // {
    //     pitch = value;
    //     updateFreq();
    // }

    // /**
    //  * @brief Get the Pitch of the wave
    //  *
    //  * @return float
    //  */
    // float getPitch()
    // {
    //     return pitch;
    // }
};

#endif
