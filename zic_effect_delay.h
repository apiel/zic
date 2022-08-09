#ifndef ZIC_EFFECT_DELAY_H_
#define ZIC_EFFECT_DELAY_H_

// On teensy we might reduce this significantly
// Or find another way to store it
#ifndef DELAY_HISTORY_SEC
#define DELAY_HISTORY_SEC 5
// #define DELAY_HISTORY_SEC 1
// #define DELAY_HISTORY_SEC 0.5f
#endif

#define DELAY_HISTORY_SAMPLES (uint32_t)(DELAY_HISTORY_SEC * SAMPLE_RATE)
#define DELAY_COUNT 5

class Zic_Effect_DelayState {
protected:
    uint32_t* historyIndex;
    int16_t* history;
    // uint32_t delayIndex = 0;

public:
    float sec = 0;
    float amplitude = 0.0f;

    Zic_Effect_DelayState(uint32_t* _historyIndex, int16_t* _history)
    {
        historyIndex = _historyIndex;
        history = _history;
    }

    void set(float _sec, float _amplitude)
    {
        amplitude = _amplitude;
        sec = range(_sec, 0.01, (float)DELAY_HISTORY_SEC);
        // delayIndex = (*historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(SAMPLE_RATE * sec)) % DELAY_HISTORY_SAMPLES;
    }

    int16_t sample()
    {
        if (amplitude == 0.0) {
            return 0;
        }
        return (history[(*historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(SAMPLE_RATE * sec)) % DELAY_HISTORY_SAMPLES] * amplitude);

        // if (delayIndex++ >= DELAY_HISTORY_SAMPLES) {
        //     delayIndex = 0;
        // }
        // return (history[delayIndex] * amplitude);
    }
};

class Zic_Effect_Delay {
protected:
    int16_t history[DELAY_HISTORY_SAMPLES] = { 0 };
    uint32_t historyIndex = 0;

    Zic_Effect_DelayState states0, states1, states2, states3, states4;

public:
    Zic_Effect_Delay()
        : states0(&historyIndex, &history[0])
        , states1(&historyIndex, &history[0])
        , states2(&historyIndex, &history[0])
        , states3(&historyIndex, &history[0])
        , states4(&historyIndex, &history[0])
    {
        // states0.set(0.1, 0.5);
        // states1.set(0.2, 0.4);
        // states2.set(0.3, 0.3);
        // states3.set(0.4, 0.2);
        // states4.set(0.5, 0.1);

        states0.set(0.5, 0.5);
        states1.set(1.0, 0.4);
        states2.set(1.5, 0.3);
        states3.set(2.0, 0.2);
        states4.set(2.5, 0.1);
    }

    int16_t sample(int16_t in)
    {
        history[historyIndex] = in;

        // avoid using modulo
        if (historyIndex++ >= DELAY_HISTORY_SAMPLES) {
            historyIndex = 0;
        }
        return in
            + states0.sample()
            + states1.sample()
            + states2.sample()
            + states3.sample()
            + states4.sample();

        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(DELAY_HISTORY_SAMPLES * 0.1)) % DELAY_HISTORY_SAMPLES] * 0.40)
        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(DELAY_HISTORY_SAMPLES * 0.2)) % DELAY_HISTORY_SAMPLES] * 0.35)
        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(DELAY_HISTORY_SAMPLES * 0.3)) % DELAY_HISTORY_SAMPLES] * 0.30)
        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(DELAY_HISTORY_SAMPLES * 0.4)) % DELAY_HISTORY_SAMPLES] * 0.25)
        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(DELAY_HISTORY_SAMPLES * 0.5)) % DELAY_HISTORY_SAMPLES] * 0.20)
        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(DELAY_HISTORY_SAMPLES * 0.6)) % DELAY_HISTORY_SAMPLES] * 0.15);

        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(SAMPLE_RATE * 0.1)) % DELAY_HISTORY_SAMPLES] * 0.5)
        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(SAMPLE_RATE * 0.2)) % DELAY_HISTORY_SAMPLES] * 0.4)
        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(SAMPLE_RATE * 0.3)) % DELAY_HISTORY_SAMPLES] * 0.3)
        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(SAMPLE_RATE * 0.4)) % DELAY_HISTORY_SAMPLES] * 0.2)
        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(SAMPLE_RATE * 0.5)) % DELAY_HISTORY_SAMPLES] * 0.1);

        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(SAMPLE_RATE * 0.5)) % DELAY_HISTORY_SAMPLES] * 0.5)
        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(SAMPLE_RATE * 1.0)) % DELAY_HISTORY_SAMPLES] * 0.4)
        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(SAMPLE_RATE * 1.5)) % DELAY_HISTORY_SAMPLES] * 0.3)
        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(SAMPLE_RATE * 2.0)) % DELAY_HISTORY_SAMPLES] * 0.2)
        // + (history[(historyIndex + DELAY_HISTORY_SAMPLES - (uint32_t)(SAMPLE_RATE * 2.5)) % DELAY_HISTORY_SAMPLES] * 0.1);
    }
};

#endif
