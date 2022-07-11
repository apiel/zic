#ifndef ZIC_WAVE_FILE_H_
#define ZIC_WAVE_FILE_H_

#include "zic_file_audio.h"
#include "zic_wave_base.h"

class Zic_Wave_File : public Zic_Wave_Base {
protected:
    int16_t sample(uint32_t* _freq)
    {
        int16_t bit;
        if (isWavetable) {
            int i = (FREQ_PI * (*_freq) * time) * audioFile.sampleCount;
            audioFile.seekToSample(i % audioFile.sampleCount);
            audioFile.read(&bit, sizeof(bit));
        } else {
            audioFile.read(&bit, sizeof(bit));
        }
        return bit * amplitude / 100;
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
        setSkipSample();
    }

    void restart()
    {
        if (!isWavetable) {
            audioFile.restart();
        }
    }
};

#endif
