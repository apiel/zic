#ifndef ZIC_EFFECT_FILTER_H_
#define ZIC_EFFECT_FILTER_H_

#include <math.h>

#include "./zic_def.h"

// https://www.martin-finke.de/articles/audio-plugins-013-filter/
// https://www.musicdsp.org/en/latest/Filters/29-resonant-filter.html

class Zic_Effect_Filter {
protected:
    float buf0 = 0;
    float buf1 = 0;
    float buf2 = 0;
    float buf3 = 0;

    float Q1;
    float feedback;
    void calculateVar()
    {
        if (cutoff == 1.0) {
            feedback = resonance + resonance;
        } else {
            feedback = resonance + resonance / (1.0 - cutoff);
        }

        // Q1 is for the second filtering method
        Q1 = 1 / (resonance * resonance * 1000 + 0.7); // 1000 value set randomly, might need to find better value?!
    }

    float nextResonantFilter(float inputValue)
    {
        // https://www.musicdsp.org/en/latest/Filters/29-resonant-filter.html
        // https://www.martin-finke.de/articles/audio-plugins-013-filter/
        buf0 += cutoff * (inputValue - buf0 + feedback * (buf0 - buf1));
        buf1 += cutoff * (buf0 - buf1);
        buf2 += cutoff * (buf1 - buf2);
        buf3 += cutoff * (buf2 - buf3);
        switch (mode) {
        case FILTER_MODE_LOWPASS_12:
            return buf1;
        case FILTER_MODE_LOWPASS_24:
            return buf3;
        case FILTER_MODE_HIGHPASS_12:
            return inputValue - buf0;
        case FILTER_MODE_HIGHPASS_24:
            return inputValue - buf3;
        case FILTER_MODE_BANDPASS_12:
            return buf0 - buf1;
        case FILTER_MODE_BANDPASS_24:
            return buf0 - buf3;
        default:
            return 0;
        }
    }

    float nextStateVariableFilter(float inputValue)
    {
        // https://www.musicdsp.org/en/latest/Filters/142-state-variable-filter-chamberlin-version.html
        float lowpass = buf1 + cutoff * buf0;
        float highpass = inputValue - lowpass - Q1 * buf0;
        float bandpass = cutoff * highpass + buf0;
        float notch = highpass + lowpass;

        buf0 = bandpass;
        buf1 = lowpass;

        switch (mode) {
        case FILTER_MODE_LOWPASS_STATE_VARIABLE:
            return lowpass;
        case FILTER_MODE_HIGHPASS_STATE_VARIABLE:
            return highpass;
        case FILTER_MODE_BANDPASS_STATE_VARIABLE:
            return bandpass;
        case FILTER_MODE_NOTCH_STATE_VARIABLE:
            return notch;
        default:
            return 0;
        }
    }

public:
    enum FilterMode {
        FILTER_MODE_OFF,
        FILTER_MODE_LOWPASS_12,
        FILTER_MODE_LOWPASS_24,
        FILTER_MODE_HIGHPASS_12,
        FILTER_MODE_HIGHPASS_24,
        FILTER_MODE_BANDPASS_12,
        FILTER_MODE_BANDPASS_24,
        FILTER_MODE_LOWPASS_STATE_VARIABLE,
        FILTER_MODE_HIGHPASS_STATE_VARIABLE,
        FILTER_MODE_BANDPASS_STATE_VARIABLE,
        FILTER_MODE_NOTCH_STATE_VARIABLE,
        FILTER_MODE_COUNT,
    };

    // uint16_t frequency = (uint16_t)(SAMPLE_RATE / 2.0); // Nyquist
    uint16_t frequency = 8000;
    float cutoff = 0.99;
    float resonance = 0.0;
    uint8_t mode = FILTER_MODE_OFF;

    Zic_Effect_Filter()
    {
        // calculateVar();
        setFrequency(frequency);
    };

    float next(float inputValue)
    {
        if (mode == FILTER_MODE_OFF || inputValue == 0) {
            return inputValue;
        }

        if (mode >= FILTER_MODE_LOWPASS_STATE_VARIABLE) {
            return nextStateVariableFilter(inputValue);
        }

        return nextResonantFilter(inputValue);
    }

    Zic_Effect_Filter* setFrequency(uint16_t freq)
    {
        // Human can hear frequency between 20Hz and 20kHz. While 20 to 20,000Hz forms the absolute borders
        // of the human hearing range, our hearing is most sensitive in the 2000 - 5000 Hz frequency range.
        // But establishing the effect of sounds with frequencies under about 250 Hz has been harder. Even
        // though they're above the lower limit of 20 Hz, these low-frequency sounds tend to be either
        // inaudible or barely audible.
        frequency = range(freq, 200, 8000);
        cutoff = 2.0 * sin(M_PI * frequency / SAMPLE_RATE);
        calculateVar();

        return this;
    }

    // could set cutoff for modulation
    // but maybe it would just be better to precalculate all the possible value for the frequency...
    //
    // or use 0 to .99 to set the value and use the getFrequencyFromCutoff ???
    //
    // look up table is most likely the best option!
    Zic_Effect_Filter* setCutoff(float _cutoff)
    {
        // cutoff cannot be 1.0 else div by zero
        // cutoff = range(_cutoff, 0.01, 0.99);

        cutoff = _cutoff;
        calculateVar();

        return this;
    };

    float getFrequencyFromCutoff()
    {
        return SAMPLE_RATE * asin(cutoff / 2.0) / M_PI;
    }

    Zic_Effect_Filter* setResonance(float _res)
    {
        // resonance = range(_res, 0.00, 0.99); ??
        resonance = range(_res, 0.00, 1.00);
        calculateVar();

        return this;
    };

    Zic_Effect_Filter* setFilterMode(int8_t value)
    {
        mode = value % FILTER_MODE_COUNT;

        return this;
    }

    const char* getName()
    {
        switch (mode) {
        case FILTER_MODE_LOWPASS_12:
            return "LPF 12dB";
        case FILTER_MODE_LOWPASS_24:
            return "LPF 24dB";
        case FILTER_MODE_HIGHPASS_12:
            return "HPF 12dB";
        case FILTER_MODE_HIGHPASS_24:
            return "HPF 24dB";
        case FILTER_MODE_BANDPASS_12:
            return "BPF 12dB";
        case FILTER_MODE_BANDPASS_24:
            return "BPF 24dB";
        case FILTER_MODE_LOWPASS_STATE_VARIABLE:
            return "LPF SV";
        case FILTER_MODE_HIGHPASS_STATE_VARIABLE:
            return "HPF SV";
        case FILTER_MODE_BANDPASS_STATE_VARIABLE:
            return "BPF SV";
        case FILTER_MODE_NOTCH_STATE_VARIABLE:
            return "NOTCH SV";
        default:
            return "OFF";
        }
    }
};

#endif

/**
Some more filters....:

float gA1 = 0, gA2 = 0;
float gB0 = 1, gB1 = 0, gB2 = 0;
float gLastX2 = 0, gLastX1 = 0, gLastY2 = 0, gLastY1 = 0;

// Where Q is similar to resonance (resonance is a range of Q... 0.707 to 1.0?)
void calculateVar(float sampleRate, float frequency, float q)
{
    float k = tanf(M_PI * frequency / sampleRate);
    float norm = 1.0 / (1 + k / q + k * k);

    gB0 = k * k * norm;
    gB1 = 2.0 * gB0;
    gB2 = gB0;
    gA1 = 2 * (k * k - 1) * norm;
    gA2 = (1 - k / q + k * k) * norm;
}

int16_t next(int16_t inputValue)
{
    float out = gB0*inputValue + gB1*gLastX1 + gB2*gLastX2 - gA1*gLastY1 - gA2*gLastY2;

    gLastX2 = gLastX1;
    gLastX1 = inputValue;
    gLastY2 = gLastY1;
    gLastY1 = out;

    return out;
}

Another: LPF seem very similar to upper one
https://www.musicdsp.org/en/latest/Filters/259-simple-biquad-filter-from-apple-com.html
//cutoff_tier range 20-20000hz
//res_tier range -25/25db
//srate - sample rate

//init
mX1 = 0;
mX2 = 0;
mY1 = 0;
mY2 = 0;
pi = 22/7;

//coefficients
cutoff = cutoff_tier;
res = res_tier;

cutoff = 2 * cutoff_tier / srate;
res = pow(10, 0.05 * -res_tier);
k = 0.5 * res * sin(pi * cutoff);
c1 = 0.5 * (1 - k) / (1 + k);
c2 = (0.5 + c1) * cos(pi * cutoff);
c3 = (0.5 + c1 - c2) * 0.25;

mA0 = 2 * c3;
mA1 = 2 * 2 * c3;
mA2 = 2 * c3;
mB1 = 2 * -c2;
mB2 = 2 * c1;

//loop
output = mA0*input + mA1*mX1 + mA2*mX2 - mB1*mY1 - mB2*mY2;

mX2 = mX1;
mX1 = input;
mY2 = mY1;
mY1 = output;

This could be another one: Butterworth Optimized resonant filter
https://www.musicdsp.org/en/latest/Filters/243-butterworth-optimized-c-class.html


Or this one: MoogFilter VCF
https://www.musicdsp.org/en/latest/Filters/24-moog-vcf.html

#include "MoogFilter.h"

MoogFilter::MoogFilter()
{
fs=44100.0;

init();
}

MoogFilter::~MoogFilter()
{
}

void MoogFilter::init()
{
// initialize values
y1=y2=y3=y4=oldx=oldy1=oldy2=oldy3=0;
calc();
};

void MoogFilter::calc()
{
float f = (cutoff+cutoff) / fs; //[0 - 1]
p=f*(1.8f-0.8f*f);
k=p+p-1.f;

float t=(1.f-p)*1.386249f;
float t2=12.f+t*t;
r = res*(t2+6.f*t)/(t2-6.f*t);
};

float MoogFilter::process(float input)
{
// process input
x = input - r*y4;

//Four cascaded onepole filters (bilinear transform)
y1= x*p +  oldx*p - k*y1;
y2=y1*p + oldy1*p - k*y2;
y3=y2*p + oldy2*p - k*y3;
y4=y3*p + oldy3*p - k*y4;

//Clipper band limited sigmoid
y4-=(y4*y4*y4)/6.f;

oldx = x; oldy1 = y1; oldy2 = y2; oldy3 = y3;
return y4;
}

float MoogFilter::getCutoff()
{ return cutoff; }

void MoogFilter::setCutoff(float c)
{ cutoff=c; calc(); }

float MoogFilter::getRes()
{ return res; }

void MoogFilter::setRes(float r)
{ res=r; calc(); }

 */
