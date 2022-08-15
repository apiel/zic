#ifndef ZIC_WAVE_FILE_H_
#define ZIC_WAVE_FILE_H_

#include "zic_file_audio.h"
#include "zic_wave_base.h"

// TODO coundlt we just cache the incrementation step/rate per frequency
// as we most likely always use same frequencies base on musical notes...
// and then always increment with this step

// TODO use linear interpolation for the wavetable?

class Zic_Wave_File : public Zic_Wave_Base {
protected:
    float sampleIndex = 0.0f;
    float sampleStep = 0.0f;
    uint64_t sampleCount = 0;

    int16_t sample()
    {
        int16_t bit = 0;
        if (isWavetable) {
            sampleIndex += sampleStep;
            while (sampleIndex >= sampleCount) {
                sampleIndex -= sampleCount;
            }
            audioFile.seekToSample((uint64_t)sampleIndex);
        }
        audioFile.read(&bit, sizeof(bit));

        return bit * amplitude / 100;
    }

    void frequencyUpdated() override
    {
        sampleStep = sampleCount * frequency / SAMPLE_RATE;
    }

    bool setSkipSample() override
    {
        skipSample = Zic_Wave_Base::setSkipSample() || !audioFile.file;
        return skipSample;
    }

public:
    Zic_File_Audio audioFile;

    bool isWavetable = false;

    void open(const char* filename, bool _isWavetable = false)
    {
        audioFile.open(filename);
        isWavetable = _isWavetable;
        sampleCount = audioFile.sampleCount;
        setSkipSample();
    }

    void reset()
    {
        // Zic_Wave_Base::reset();
        if (!isWavetable) {
            audioFile.restart();
        }
    }
};

#endif
