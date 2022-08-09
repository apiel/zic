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

    /**
     * @brief save the current sample to the delay buffer
     *
     * @param in
     * @return int16_t return the current sample
     */
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
    float feedback = 0.0f;

    /**
     * @brief Construct a new Zic_Effect_Delay object
     *
     * @param _history a pointer to the delay history buffer
     */
    Zic_Effect_Delay(Zic_Effect_DelayHistory* _history)
        : history(_history)
    {
    }

    /**
     * @brief Set the delay time (in sec), amplitude and feedback
     *
     * @param _sec
     * @param _amplitude
     * @param _feedback
     */
    void set(float _sec, float _amplitude, float _feedback)
    {
        setSec(_sec);
        setAmplitude(_amplitude);
        setFeedback(_feedback);
    }

    /**
     * @brief Set the delay time in seconds
     *
     * @param _sec
     * @return Zic_Effect_Delay*
     */
    Zic_Effect_Delay* setSec(float _sec)
    {
        sec = range(_sec, 0.01, (float)DELAY_HISTORY_SEC);
        // delayIndex = (history->index + DELAY_BUFFER_SIZE - (uint32_t)(SAMPLE_RATE * sec)) % DELAY_BUFFER_SIZE;
        return this;
    }

    /**
     * @brief Set the amplitude
     *
     * @param _amplitude
     * @return Zic_Effect_Delay*
     */
    Zic_Effect_Delay* setAmplitude(float _amplitude)
    {
        amplitude = range(_amplitude, 0.0f, 1.0f);
        return this;
    }

    /**
     * @brief Set the feedback
     *
     * @param _feedback
     * @return Zic_Effect_Delay*
     */
    Zic_Effect_Delay* setFeedback(float _feedback)
    {
        feedback = range(_feedback, 0.0f, 1.0f);
        return this;
    }

    /**
     * @brief return the delayed sample
     *
     * @return int16_t
     */
    int16_t sample()
    {
        if (amplitude == 0.0) {
            return 0;
        }
        int16_t delay = (history->buffer[(history->index + DELAY_BUFFER_SIZE - (uint32_t)(SAMPLE_RATE * sec)) % DELAY_BUFFER_SIZE] * amplitude);

        // if (delayIndex++ >= DELAY_BUFFER_SIZE) {
        //     delayIndex = 0;
        // }
        // int16_t delay = (history->buffer[delayIndex] * amplitude);

        if (feedback > 0.0) {
            history->buffer[history->index] += delay * feedback;
        }
        return delay;
    }
};

#endif
