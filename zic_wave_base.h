#ifndef ZIC_WAVE_BASE_H_
#define ZIC_WAVE_BASE_H_

#include <math.h>

#include "zic_def.h"
#include "zic_note.h"

class Zic_Wave_Base {
protected:
    float time = 0.0;

    virtual float sample() { return 0; }
    float frequency = 103.82617439443122f; // C3

    bool skipSample = false;
    bool mute = false;
    float level = 1.0;
    float amplitude = 1.0;

    virtual bool setSkipSample()
    {
        skipSample = mute || amplitude <= 0.0;
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
    float next()
    {
        return next(0.0, 0.0, 0.0);
    }

    float next(float modAmplitude, float modPitch, float modMorph)
    {
        if (skipSample) {
            return 0;
        }

        return level * sample();
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
     * @brief Set the Amplitude of the wave
     *
     * @param value between 0.0 and 1.0
     */
    void setAmplitude(float value)
    {
        amplitude = range(value, 0.0, 1.0);
        setLevel();
    }

    /**
     * @brief Get the Amplitude of the wave
     *
     * @return float
     */
    float getAmplitude()
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
        frequency = range(value, 0.01f, 80000.0f);
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
