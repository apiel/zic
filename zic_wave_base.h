#ifndef ZIC_WAVE_BASE_H_
#define ZIC_WAVE_BASE_H_

#include <math.h>

#include "zic_def.h"
#include "zic_note.h"

class Zic_Wave_Base {
protected:
    float time = 0.0;

    virtual int16_t sample() { return 0; }
    float frequency = 103.82617439443122f; // C3

    bool skipSample = false;
    bool mute = false;
    int8_t level = 100;
    int8_t amplitude = 100;
    // int8_t velocity = 100;
    // float pitch = 1.0f;

    uint16_t phase = 0; // 0 to 360

    virtual bool setSkipSample()
    {
        skipSample = mute || amplitude <= 0;
        return skipSample;
    }

    virtual void frequencyUpdated()
    {
    }

    void setLevel(float _level = 1.0f)
    {
        level = amplitude * _level;
        setSkipSample();
    }

public:
    int16_t next()
    {
        if (skipSample) {
            return 0;
        }

        // use bitwise >> 8 to reduce amplitude (division by 256)
        // we could have a higher quality wavetable to int32 using a higher bitwise value
        // but is it really necessary? int16 make a gain on firmware size!
        return (level * sample()) >> 8;
    }

    /**
     * @brief Set wave on mute
     *
     * @param _mute
     */
    void setMute(bool _mute = true)
    {
        mute = _mute;
        setSkipSample();
    }

    /**
     * @brief return muted status
     *
     * @return true
     * @return false
     */
    bool isMuted()
    {
        return mute;
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
        setLevel();
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

    void setVelocity(int8_t value)
    {
        setLevel((float)value / 127.0f);
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
        frequency = value;
        frequencyUpdated();
    }

    /**
     * @brief Get the Frequency of the wave
     *
     * @return float
     */
    float getFrequency()
    {
        return frequency;
    }
};

#endif
