#ifndef ZIC_MOD_ASR_H_
#define ZIC_MOD_ASR_H_

#include "zic_def.h"
#include <stdint.h>

// FIXME still make a cutting noise


template <typename T, typename TData = int16_t>
class Zic_Mod_AsrBase {
protected:
    uint8_t phase = END_PHASE;
    uint8_t nextPhase = END_PHASE;
    uint16_t attackMs = 10;
    uint16_t releaseMs = 500;

    T stepTarget = 1.0f;
    T attackStep;
    T releaseStep;
    T value = 0;

    uint8_t note = 0;

    virtual int16_t getData(int16_t data)
    {
        return value * data;
    }

public:
    enum {
        ATTACK_PHASE,
        SUSTAIN_PHASE,
        RELEASE_PHASE,
        END_PHASE
    };

    /**
     * @brief set to true to skip substain phase
     */
    bool noSustain = false;

    Zic_Mod_AsrBase(bool _noSustain = false)
    {
        noSustain = _noSustain;
        setAttack(attackMs);
        setRelease(releaseMs);
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
     * @brief Set the Attack in millisecond.
     *
     * @param ms
     */
    virtual void setAttack(uint16_t ms)
    {
        attackMs = ms;
        attackStep = stepTarget / ((float)ms * SAMPLE_PER_MS);
    }

    /**
     * @brief Return the Attack in millisecond.
     *
     * @param ms
     */
    uint16_t getAttack()
    {
        return attackMs;
    }

    /**
     * @brief Set the Release in millisecond.
     *
     * @param ms
     */
    virtual void setRelease(uint16_t ms)
    {
        releaseMs = ms;
        releaseStep = stepTarget / ((float)ms * SAMPLE_PER_MS);
    }

    /**
     * @brief Return the Release in millisecond.
     *
     * @param ms
     */
    uint16_t getRelease()
    {
        return releaseMs;
    }

    virtual void debug()
    {
        // TODO add generic print
        // Serial.print("stepTarget ");
        // Serial.println(stepTarget);
        // Serial.printf("attack %d step ", attackMs);
        // Serial.println(attackStep);
        // Serial.printf("release %d step ", releaseMs);
        // Serial.println(releaseStep);
    }

    /**
     * @brief To be call on each sample to update the phase. If a data value is passed as param,
     * it will be returned with the envelop level applied to it. Else it return a value between
     * 0 and 100.
     *
     * @param data
     * @return int16_t
     */
    TData next(TData data = 100)
    {
        switch (phase) {
        case ATTACK_PHASE:
            if (value < stepTarget) {
                value += attackStep;
            } else {
                // Serial.println("end ATTACK_PHASE");
                value = stepTarget;
                phase = nextPhase;
                nextPhase = END_PHASE;
            }
            break;
        case SUSTAIN_PHASE:
            value = stepTarget;
            break;
        case RELEASE_PHASE:
            if (value > releaseStep) {
                value -= releaseStep;
            } else {
                // Serial.println("end RELEASE_PHASE");
                value = 0;
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
     *
     * @param _note
     */
    void on(uint8_t _phase = ATTACK_PHASE, uint8_t _note = 0)
    {
        // Serial.println("start ATTACK_PHASE");
        note = _note;
        value = 0;
        nextPhase = noSustain ? RELEASE_PHASE : SUSTAIN_PHASE;
        phase = _phase;
    }

    /**
     * @brief Skip attack phase and go directly to sustain phase
     *
     * @param _note
     */
    void tie(uint8_t _note = 0)
    {
        on(SUSTAIN_PHASE, _note);
    }

    /**
     * @brief Stop to play the envelop
     *
     * @param _note
     */
    void off(uint8_t _note = 0)
    {
        if (_note && _note != note) {
            return;
        }

        phase = RELEASE_PHASE;
    }

    /**
     * @brief Stop to play the envelop but ensure that attack phase complete before to start release phase.
     *
     * @param _note
     */
    void nextOff(uint8_t _note = 0)
    {
        if (_note && _note != note) {
            return;
        }

        if (nextPhase != END_PHASE) {
            nextPhase = RELEASE_PHASE;
        } else {
            off(_note);
        }
    }
};

/**
 * @brief ASR envelop modulation
 * 
 */
class Zic_Mod_Asr : public Zic_Mod_AsrBase<float>
{
};

#endif
