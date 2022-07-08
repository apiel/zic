#ifndef ZIC_WAVE_FILE_H_
#define ZIC_WAVE_FILE_H_

#include "zic_file_audio.h"
#include "zic_wave_base.h"

class Zic_Wave_File : public Zic_Wave_Base {
protected:
    int16_t sample(uint32_t* _freq)
    {
        if (!audioFile.file) {
            return 0;
        }

        int16_t bit;
        if (isWavetable) {
            int i = (FREQ_PI * (*_freq) * time) * audioFile.sampleCount;
            audioFile.seekToSample(i % audioFile.sampleCount);
            audioFile.read(&bit, sizeof(bit));
        } else {
            audioFile.read(&bit, sizeof(bit));
        }
        return bit;
    }

public:
    Zic_File_Audio audioFile;

    bool isWavetable = false;

    Zic_Wave_File()
    {
        // open("samples/kick.wav");
        open("wavetables/59.wav", true);
    }

    void open(const char* filename, bool _isWavetable = false)
    {
        audioFile.open(filename);
        isWavetable = _isWavetable;
    }

    void restart()
    {
        audioFile.restart();
    }
};

#endif
