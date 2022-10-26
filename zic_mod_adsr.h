#ifndef ZIC_MOD_ADSR_H_
#define ZIC_MOD_ADSR_H_

#include <stdint.h>

#include "zic_def.h"
class Zic_Mod_Adsr {
protected:
    enum {
        ATTACK_PHASE,
        DECAY_PHASE,
        SUSTAIN_PHASE,
        RELEASE_PHASE,
        END_PHASE
    };

    enum {
        ATTACK_MS,
        DECAY_MS,
        RELEASE_MS,
        COUNT_MS,
    };

    uint8_t phase = END_PHASE;
    uint8_t nextPhase = END_PHASE;
    uint16_t ms[COUNT_MS] = { 10, 100, 500 };
    float steps[COUNT_MS] = { 0.0f, 0.0f, 0.0f };

    float sustainTarget = 0.5f;
    float stepTarget = 1.0f;
    float value = 0;

    virtual int16_t getData(int16_t data)
    {
        return value * data;
    }

public:
    /**
     * @brief set to true to skip substain phase
     */
    bool noSustain = false;

    Zic_Mod_Adsr(bool _noSustain = false)
    {
        noSustain = _noSustain;
        setAttack(getAttack());
        setDecay(getDecay());
        setRelease(getRelease());
    }

    /**
     * @brief return if the envelop is currently playing
     *
     * @return true
     * @return false
     */
    bool isOn()
    {
        return phase != END_PHASE;
    }

    /**
     * @brief Set the Sustain target level
     */
    virtual void setSustain(float target)
    {
        sustainTarget = range(target, 0.0f, 1.0f);
    }

    /**
     * @brief Get the Sustain level
     *
     * @return float
     */
    float getSustain()
    {
        return sustainTarget;
    }

    /**
     * @brief Set the Attack in millisecond.
     *
     * @param ms
     */
    virtual void setAttack(int16_t _ms)
    {
        ms[ATTACK_MS] = range(_ms, 0, 9900);
        steps[ATTACK_MS] = stepTarget / ((float)_ms * SAMPLE_PER_MS);
    }

    /**
     * @brief Return the Attack in millisecond.
     *
     * @param ms
     */
    uint16_t getAttack()
    {
        return ms[ATTACK_MS];
    }

    /**
     * @brief Set the Decay in millisecond.
     *
     * @param ms
     */
    virtual void setDecay(int16_t _ms)
    {
        ms[DECAY_MS] = range(_ms, 0, 9900);
        steps[DECAY_MS] = stepTarget / ((float)_ms * SAMPLE_PER_MS);
    }

    /**
     * @brief Return the Decay in millisecond.
     *
     * @return uint16_t
     */
    uint16_t getDecay()
    {
        return ms[DECAY_MS];
    }

    /**
     * @brief Set the Release in millisecond.
     *
     * @param ms
     */
    virtual void setRelease(int16_t _ms)
    {
        ms[RELEASE_MS] = range(_ms, 0, 9900);
        steps[RELEASE_MS] = stepTarget / ((float)_ms * SAMPLE_PER_MS);
    }

    /**
     * @brief Return the Release in millisecond.
     *
     * @param ms
     */
    uint16_t getRelease()
    {
        return ms[RELEASE_MS];
    }

    /**
     * @brief To be call on each sample to update the phase. If a data value is passed as param,
     * it will be returned with the envelop level applied to it. Else it return a value between
     * 0 and 100.
     *
     * @param data
     * @return int16_t
     */
    int16_t next(int16_t data = 100)
    {
        switch (phase) {
        case ATTACK_PHASE:
            if (value < stepTarget) {
                value += steps[ATTACK_MS];
            } else {
                value = stepTarget;
                phase = DECAY_PHASE;
                stepTarget = sustainTarget;
            }
            break;
        case DECAY_PHASE:
            if (value > stepTarget) {
                value -= steps[DECAY_MS];
            } else {
                phase = noSustain ? RELEASE_PHASE : SUSTAIN_PHASE;
                value = stepTarget;
                stepTarget = 0.0f;
            }
            break;
        case SUSTAIN_PHASE:
            break;
        case RELEASE_PHASE:
            if (value > stepTarget) {
                value -= steps[RELEASE_MS];
            } else {
                value = 0.0f;
                phase = END_PHASE;
            }
            break;
        case END_PHASE:
            break;
        }

        return getData(data);
    }

    /**
     * @brief Start to play the envelop
     */
    void on()
    {
        value = 0;
        phase = ATTACK_PHASE;
        stepTarget = 1.0f;
    }

    /**
     * @brief Stop to play the envelop
     */
    void off()
    {
        if (phase != END_PHASE) {
            phase = RELEASE_PHASE;
            stepTarget = 0.0f;
        }
    }
};

#endif
