#ifndef ZIC_MOD_FILTER_H_
#define ZIC_MOD_FILTER_H_

// http://www.martin-finke.de/blog/articles/audio-plugins-013-filter/

class Zic_Mod_Filter
{
protected:
    enum FilterMode
    {
        FILTER_MODE_LOWPASS = 0,
        FILTER_MODE_HIGHPASS,
        FILTER_MODE_BANDPASS,
        kNumFilterModes
    };

    float buf0 = 0.0;
    float buf1 = 0.0;
    float buf2 = 0.0;
    float buf3 = 0.0;

    float feedbackAmount;
    void calculateFeedbackAmount()
    {
        feedbackAmount = resonance + resonance / (1.0 - cutoff);
    }

public:
    float cutoff = 0.2;
    float resonance = 0.9;
    FilterMode mode = FILTER_MODE_LOWPASS;

    Zic_Mod_Filter()
    {
        calculateFeedbackAmount();
    };

    int16_t next(int16_t inputValue)
    {
        if (inputValue == 0)
            return inputValue;
        buf0 += cutoff * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));
        buf1 += cutoff * (buf0 - buf1);
        buf2 += cutoff * (buf1 - buf2);
        buf3 += cutoff * (buf2 - buf3);
        switch (mode)
        {
        case FILTER_MODE_LOWPASS:
            return buf3;
        case FILTER_MODE_HIGHPASS:
            return inputValue - buf3;
        case FILTER_MODE_BANDPASS:
            return buf0 - buf3;
        default:
            return 0.0;
        }
    }

    // TODO
    // on lower freq "value" should be more step, so going expodentially down
    // at least for LPF, might be the opposite for HPF
    void setCutoff(float _cutoff)
    {
        set(_cutoff, resonance);
    };

    void setResonance(float _res)
    {
        set(cutoff, _res);
    };

    void set(float _cutoff, float _res)
    {
        cutoff = _cutoff;
        resonance = _res;
        calculateFeedbackAmount();
    }

    void setFilterMode(FilterMode value)
    {
        mode = value;
    }
};

#endif
