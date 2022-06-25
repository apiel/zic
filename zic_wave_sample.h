#ifndef ZIC_WAVE_SAMPLE_H_
#define ZIC_WAVE_SAMPLE_H_

#include "zic_file_audio.h"
#include "zic_wave_base.h"

class Zic_Wave_Sample : public Zic_Wave_Base {
protected:
    Zic_File_Audio audioFile;

    int16_t sample(uint32_t* _freq)
    {
        if (!audioFile.file) {
            return 0;
        }
        int16_t bit;
        bool r = audioFile.read(&bit, sizeof(bit));
        if (!r) {
            onEnd();
        }
        return bit;
    }

public:
    bool loop = false;

    Zic_Wave_Sample()
    {
        // file = zic_file_open("samples/59.wav", "rb");
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
};

#endif
