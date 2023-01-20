#ifndef ZIC_EFFECT_DISTORTION_H_
#define ZIC_EFFECT_DISTORTION_H_

#include <math.h>
#include <stdint.h>

#include "./zic_def.h"

#define ZIC_WAVESHAPE_SIZE 4097

class Zic_Effect_Distortion {
protected:
    float distortionAmount = 0.0;
    float distortionRange = 20.0;

    float deg = M_PI / 180.0;

public:
    int16_t distLerpshift; // 4
    int16_t distWaveshape[ZIC_WAVESHAPE_SIZE];

    Zic_Effect_Distortion()
    {
        setDistortion(distortionAmount);

        int index = ZIC_WAVESHAPE_SIZE - 1;
        distLerpshift = 16;
        while (index >>= 1) {
            --distLerpshift;
        }
    }

    void setDistortion(float _amount)
    {
        distortionAmount = _amount;
        if (distortionAmount > 100.0) {
            distortionAmount = 100.0;
        } else if (distortionAmount < 0.0) {
            distortionAmount = 0.0;
        }
        for (u_int16_t i = 0; i < ZIC_WAVESHAPE_SIZE; i++) {
            float x = (float)i * 2.0 / (float)ZIC_WAVESHAPE_SIZE - 1.0;
            float waveshape = (3 + distortionAmount) * x * distortionRange * deg / (M_PI + distortionAmount * abs(x));
            distWaveshape[i] = 32767 * waveshape;
        }
    }

    void setDistortionRange(float _range)
    {
        if (_range < 10.0) {
            _range = 10.0;
        } else if (_range > 120.0) {
            _range = 120.0;
        }

        distortionRange = _range;
        setDistortion(distortionAmount);
    }

    float next(float in)
    {
        if (distortionAmount <= 0.0) {
            return in;
        }

        uint16_t x = in * 32768 + 32768;
        uint16_t xa = x >> distLerpshift;
        int16_t ya = distWaveshape[xa];
        int16_t yb = distWaveshape[xa + 1];
        x = ya + ((yb - ya) * (x - (xa << distLerpshift)) >> distLerpshift);

        return x / 32768.0f;
    }
};

#endif
