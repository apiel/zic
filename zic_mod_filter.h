#ifndef ZIC_MOD_FILTER_H_
#define ZIC_MOD_FILTER_H_

// http://www.martin-finke.de/blog/articles/audio-plugins-013-filter/

class Zic_Mod_Filter
{
protected:
    enum FilterMode
    {
        FILTER_MODE_OFF,
        FILTER_MODE_LOWPASS,
        FILTER_MODE_HIGHPASS,
        FILTER_MODE_BANDPASS,
        FILTER_MODE_COUNT,
    };

    float buf0 = 0.0;
    float buf1 = 0.0;
    float buf2 = 0.0;
    float buf3 = 0.0;

    float cutoffCalc = 0.2;
    float feedbackAmount;
    void calculateVar()
    {
        cutoffCalc = cutoff;
        // cutoffCalc = 1.0f - cutoff * cutoff * cutoff;
        feedbackAmount = resonance + resonance / (1.0 - cutoffCalc);
        Serial.printf("cutoff %.2f calc %.2f res %.2f feedback %.2f\n", cutoff, cutoffCalc, resonance, feedbackAmount);
    }

public:
    float cutoff = 0.2;
    float resonance = 0.9;
    FilterMode mode = FILTER_MODE_LOWPASS;

    Zic_Mod_Filter()
    {
        calculateVar();
    };

    int16_t next(int16_t inputValue)
    {
        if (mode == FILTER_MODE_OFF || inputValue == 0)
        {
            return inputValue;
        }
        buf0 += cutoffCalc * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));
        buf1 += cutoffCalc * (buf0 - buf1);
        buf2 += cutoffCalc * (buf1 - buf2);
        buf3 += cutoffCalc * (buf2 - buf3);
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
        cutoff = range(_cutoff, 0.01, 0.99);
        resonance = range(_res, 0.00, 0.99);
        calculateVar();
    }

    void setFilterMode(FilterMode value)
    {
        mode = value;
    }
};

#endif
