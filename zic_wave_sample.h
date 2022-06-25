#ifndef ZIC_WAVE_SAMPLE_H_
#define ZIC_WAVE_SAMPLE_H_

#include "zic_file_audio.h"
#include "zic_wave_base.h"

class Zic_Wave_Sample : public Zic_Wave_Base {
protected:
    Zic_File_Audio audioFile;

    int16_t sample(uint32_t* _freq)
    {
        int16_t bit;
        bool r = zic_file_read(audioFile.file, &bit, sizeof(bit)); // sizeof(int16_t)
        if (!r) {
            zic_file_seek(audioFile.file, 0);
        }
        // printf("bit: %d\n", bit);
        return bit;
        // return 0;
    }

public:
    Zic_Wave_Sample()
    {
        // file = zic_file_open("samples/59.wav", "rb");
        // // file = zic_file_open("samples/kick.wav", "rb");
    }
};

#endif
