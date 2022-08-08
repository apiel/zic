#ifndef ZIC_EFFECT_DELAY_H_
#define ZIC_EFFECT_DELAY_H_

// On teensy we might reduce this significantly
// Or find another way to store it
#ifndef APP_AUDIO_TRACK_HISTORY_SEC
#define APP_AUDIO_TRACK_HISTORY_SEC 5
// #define APP_AUDIO_TRACK_HISTORY_SEC 1
// #define APP_AUDIO_TRACK_HISTORY_SEC 0.5f
#endif

#define APP_AUDIO_TRACK_HISTORY_SAMPLES (uint32_t)(APP_AUDIO_TRACK_HISTORY_SEC * SAMPLE_RATE)

class Zic_Effect_Delay {
protected:
    int16_t history[APP_AUDIO_TRACK_HISTORY_SAMPLES] = { 0 };
    uint32_t historyIndex = 0;

public:
    int16_t sample(int16_t in)
    {
        history[historyIndex] = in;

        // avoid using modulo
        // historyIndex = (historyIndex + 1) % APP_AUDIO_TRACK_HISTORY_SAMPLES;
        // instead
        if (historyIndex++ >= APP_AUDIO_TRACK_HISTORY_SAMPLES) {
            historyIndex = 0;
        }
        return in
            // + (d * 0.80);
            // + (history[(historyIndex + APP_AUDIO_TRACK_HISTORY_SAMPLES - (uint32_t)(APP_AUDIO_TRACK_HISTORY_SAMPLES * 0.1)) % APP_AUDIO_TRACK_HISTORY_SAMPLES] * 0.60);

            + (history[(historyIndex + APP_AUDIO_TRACK_HISTORY_SAMPLES - (uint32_t)(APP_AUDIO_TRACK_HISTORY_SAMPLES * 0.1)) % APP_AUDIO_TRACK_HISTORY_SAMPLES] * 0.40)
            + (history[(historyIndex + APP_AUDIO_TRACK_HISTORY_SAMPLES - (uint32_t)(APP_AUDIO_TRACK_HISTORY_SAMPLES * 0.2)) % APP_AUDIO_TRACK_HISTORY_SAMPLES] * 0.35)
            + (history[(historyIndex + APP_AUDIO_TRACK_HISTORY_SAMPLES - (uint32_t)(APP_AUDIO_TRACK_HISTORY_SAMPLES * 0.3)) % APP_AUDIO_TRACK_HISTORY_SAMPLES] * 0.30)
            + (history[(historyIndex + APP_AUDIO_TRACK_HISTORY_SAMPLES - (uint32_t)(APP_AUDIO_TRACK_HISTORY_SAMPLES * 0.4)) % APP_AUDIO_TRACK_HISTORY_SAMPLES] * 0.25)
            + (history[(historyIndex + APP_AUDIO_TRACK_HISTORY_SAMPLES - (uint32_t)(APP_AUDIO_TRACK_HISTORY_SAMPLES * 0.5)) % APP_AUDIO_TRACK_HISTORY_SAMPLES] * 0.20)
            + (history[(historyIndex + APP_AUDIO_TRACK_HISTORY_SAMPLES - (uint32_t)(APP_AUDIO_TRACK_HISTORY_SAMPLES * 0.6)) % APP_AUDIO_TRACK_HISTORY_SAMPLES] * 0.15);

        // + (history[(historyIndex + (uint32_t)(APP_AUDIO_TRACK_HISTORY_SAMPLES * 0.9)) % APP_AUDIO_TRACK_HISTORY_SAMPLES] * 0.40)
        // + (history[(historyIndex + (uint32_t)(APP_AUDIO_TRACK_HISTORY_SAMPLES * 0.8)) % APP_AUDIO_TRACK_HISTORY_SAMPLES] * 0.35)
        // + (history[(historyIndex + (uint32_t)(APP_AUDIO_TRACK_HISTORY_SAMPLES * 0.7)) % APP_AUDIO_TRACK_HISTORY_SAMPLES] * 0.30)
        // + (history[(historyIndex + (uint32_t)(APP_AUDIO_TRACK_HISTORY_SAMPLES * 0.6)) % APP_AUDIO_TRACK_HISTORY_SAMPLES] * 0.25)
        // + (history[(historyIndex + (uint32_t)(APP_AUDIO_TRACK_HISTORY_SAMPLES * 0.5)) % APP_AUDIO_TRACK_HISTORY_SAMPLES] * 0.20)
        // + (history[(historyIndex + (uint32_t)(APP_AUDIO_TRACK_HISTORY_SAMPLES * 0.4)) % APP_AUDIO_TRACK_HISTORY_SAMPLES] * 0.15);
    }
};

#endif
