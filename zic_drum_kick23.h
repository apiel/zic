#ifndef ZIC_DRUM_KICK23_H_
#define ZIC_DRUM_KICK23_H_

#include <stdint.h>

#include "./zic_def.h"
#include "./zic_effect_distortion.h"

#define ENVELOP_STEPS 6
#define POT_COUNT 3

class Zic_Drum_Kick23 {
protected:
    float frequency = 600.0f;
    float morph = 0.0f;
    unsigned int wavetablePos = 0;
    unsigned int duration = 300; // in ms
    float volume = 1.0f;

    unsigned int sampleCountDuration = duration * SAMPLE_PER_MS;
    unsigned int sampleCount = -1; // set it to max uint value so it will not trigger the kick at the beginning

    // The first 2 steps are readonly, so for amp env there is very short ramp up to avoid clicking noize
    // The last step is also readonly, so the amp and freq end to 0.0f
    float envelopAmp[ENVELOP_STEPS][2] = { { 0.0f, 0.0f }, { 1.0f, 0.01f }, { 0.3f, 0.4f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f } };
    float envelopFreq[ENVELOP_STEPS][2] = { { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.26f, 0.03f }, { 0.24f, 0.35f }, { 0.22f, 0.4f }, { 0.0f, 1.0f } };

    unsigned int envelopAmpIndex = 0;
    unsigned int envelopFreqIndex = 0;

    Zic_Effect_Distortion distortion;

    float sampleIndex = 0.0f;

public:
    float envelop(float (*envelop)[2], unsigned int* envelopIndex)
    {
        if (envelopFreqIndex > ENVELOP_STEPS - 1) {
            return 0.0f;
        }

        float time = (float)sampleCount / (float)sampleCountDuration;
        if (time >= envelop[*envelopIndex + 1][1]) {
            (*envelopIndex)++;
        }
        float timeOffset = envelop[*envelopIndex + 1][1] - envelop[*envelopIndex][1];
        float timeRatio = (time - envelop[*envelopIndex][1]) / timeOffset;
        return (envelop[*envelopIndex + 1][0] - envelop[*envelopIndex][0]) * timeRatio + envelop[*envelopIndex][0];
    }

    void noteon()
    {
        sampleCount = 0;
        envelopAmpIndex = 0;
        envelopFreqIndex = 0;
    }

    float sample()
    {
        if (sampleCount < sampleCountDuration) {
            float envFreq = envelop(envelopFreq, &envelopFreqIndex);
            float envAmp = envelop(envelopAmp, &envelopAmpIndex);

            sampleIndex += WAVETABLE_SIZE * (frequency * envFreq) / SAMPLE_RATE;
            while (sampleIndex >= WAVETABLE_SIZE) {
                sampleIndex -= WAVETABLE_SIZE;
            }
            sampleCount++;
            return distortion.next(wavetable[wavetablePos + (uint16_t)sampleIndex] * envAmp * volume);
        }
        return 0;
    }

    void updateMorph(float _morph)
    {
        morph = _morph;
        if (morph < 0.0f) {
            morph = 0.0f;
        } else if (morph > WAVETABLE_COUNT) {
            morph = WAVETABLE_COUNT;
        }
        wavetablePos = (uint16_t)(morph / WAVETABLE_COUNT * (WAVETABLE_TOTAL_SIZE - WAVETABLE_SIZE));
    }

    /**
     * @brief Update the duration of the kick (total envelope time)
     *
     * @param _duration
     */
    void updateDuration(unsigned int _duration)
    {
        duration = _duration;
        if (duration < 10) {
            duration = 10;
        } else if (duration > 5000) {
            duration = 5000;
        }
        sampleCountDuration = duration * SAMPLE_PER_MS;
    }

    void updateVolume(float _volume)
    {
        volume = _volume;
        if (volume < 0.0f) {
            volume = 0.0f;
        } else if (volume > 1.0f) {
            volume = 1.0f;
        }
    }

    void updateFrequency(float _frequency)
    {
        frequency = _frequency;
        if (frequency < 10.0f) {
            frequency = 0.0f;
        } else if (frequency > 2000.0f) {
            frequency = 20000.0f;
        }
    }
};

#endif
