#ifndef ZIC_SEQ_TEMPO_H_
#define ZIC_SEQ_TEMPO_H_

#include "zic_def.h"

// 4 𝆺𝅥𝅯𝆺𝅥𝅯𝆺𝅥𝅯𝆺𝅥𝅯 step per beat
template <uint8_t STEP_PER_BEAT = 4>
class Zic_Seq_Tempo {
protected:
    unsigned long previous = 0;
    uint16_t tempo;
    uint16_t bpm;
    uint16_t sampleCount = 0;
    unsigned long time;

public:
    Zic_Seq_Tempo()
    {
        set(80);
    }

    Zic_Seq_Tempo(uint16_t _bpm)
    {
        set(_bpm);
    }

    /**
     * @brief Set tempo in beat per minutes (BPM)
     *
     * @param _bpm
     */
    void set(uint16_t _bpm)
    {
        bpm = range(_bpm, 10, 250);
        tempo = 60000.0f / (float)(bpm * STEP_PER_BEAT);
        printf("Tempo (%d -> %d): %d\n", _bpm, bpm, tempo);
    }

    /**
     * @brief Get tempo in beat per minutes (BPM)
     *
     * @return uint16_t
     */
    uint16_t getBpm()
    {
        return bpm;
    }

    /**
     * @brief Tempo base on given time
     *
     * @param now
     * @return true
     * @return false
     */
    bool next(unsigned long now)
    {
        if ((now - previous) >= tempo) {
            previous = now;
            return true;
        }
        return false;
    }

    /**
     * @brief Tempo base on sample rate
     *
     * @return true
     * @return false
     */
    bool next()
    {
        sampleCount += APP_AUDIO_CHUNK / APP_CHANNELS;
        if (sampleCount > SAMPLE_PER_MS) {
            time += sampleCount / (SAMPLE_PER_MS);
            sampleCount = 0;
            return next(time);
        }
        return false;
    }
};

#endif