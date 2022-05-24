#ifndef ZIC_SEQ_TEMPO_H_
#define ZIC_SEQ_TEMPO_H_

#include "zic_def.h"

// 4 ♬ ♬ step per beat
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

    void set(uint16_t _bpm)
    {
        bpm = _bpm;
        tempo = 60000 / (bpm * STEP_PER_BEAT);
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
        sampleCount++;
        if (sampleCount > SAMPLE_PER_MS) {
            sampleCount = 0;
            time++;
            return next(time);
        }
        return false;
    }
};

#endif