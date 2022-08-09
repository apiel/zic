#ifndef ZIC_EFFECT_DELAY_H_
#define ZIC_EFFECT_DELAY_H_

// On teensy we might reduce this significantly
// Or find another way to store it
#ifndef DELAY_HISTORY_SEC
#define DELAY_HISTORY_SEC 5
// #define DELAY_HISTORY_SEC 1
// #define DELAY_HISTORY_SEC 0.5f
#endif

#define DELAY_BUFFER_SIZE (uint32_t)(DELAY_HISTORY_SEC * SAMPLE_RATE)
#define DELAY_COUNT 5

class Zic_Effect_DelayHistory {
public:
    int16_t buffer[DELAY_BUFFER_SIZE] = { 0 };
    uint32_t index = 0;

    int16_t sample(int16_t in)
    {
        buffer[index] = in;

        // avoid using modulo
        if (index++ >= DELAY_BUFFER_SIZE) {
            index = 0;
        }
        return in;
    }
};

class Zic_Effect_Delay {
protected:
    Zic_Effect_DelayHistory* history;
    // uint32_t delayIndex = 0;

public:
    float sec = 0;
    float amplitude = 0.0f;

    Zic_Effect_Delay(Zic_Effect_DelayHistory* _history)
        : history(_history)
    {
    }

    void set(float _sec, float _amplitude)
    {
        amplitude = _amplitude;
        sec = range(_sec, 0.01, (float)DELAY_HISTORY_SEC);
        // delayIndex = (history->index + DELAY_BUFFER_SIZE - (uint32_t)(SAMPLE_RATE * sec)) % DELAY_BUFFER_SIZE;
    }

    int16_t sample()
    {
        if (amplitude == 0.0) {
            return 0;
        }
        return (history->buffer[(history->index + DELAY_BUFFER_SIZE - (uint32_t)(SAMPLE_RATE * sec)) % DELAY_BUFFER_SIZE] * amplitude);

        // if (delayIndex++ >= DELAY_BUFFER_SIZE) {
        //     delayIndex = 0;
        // }
        // return (history->buffer[delayIndex] * amplitude);
    }
};

#endif
