#ifndef ZIC_WAVE_SAMPLE_H_
#define ZIC_WAVE_SAMPLE_H_

#include "zic_file_audio.h"
#include "zic_wave_base.h"

class Zic_Wave_File : public Zic_Wave_Base {
protected:
    int16_t sample(uint32_t* _freq)
    {
        if (!audioFile.file) {
            return 0;
        }

        // // this seemed to work with wavetable samples, but not with basic wav samples
        // // but does not work :p
        // int i = (FREQ_PI * (*_freq) * time) * audioFile.sampleCount;
        // audioFile.seekToSample(i % audioFile.sampleCount);

        int16_t bit;
        bool r = audioFile.read(&bit, sizeof(bit));
        if (!r) {
            onEnd();
        }
        return bit;
    }

public:
    Zic_File_Audio audioFile;

    bool loop = false;
    // bool loop = true;

    Zic_Wave_File()
    {
        // audioFile.open("samples/59.wav");
        audioFile.open("samples/kick.wav");
    }

    void onEnd()
    {
        if (loop) {
            restart();
        }
    }

    void restart()
    {
        audioFile.restart();
    }

    // void setNote(uint8_t note) override
    // {
    //     // setFrequency(Zic::NOTE_FREQ[note]);
    //     audioFile.setPitchSemiTones(Zic::_NOTE_C4 - note);
    // }
};

#endif
