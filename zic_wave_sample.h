#ifndef ZIC_WAVE_SAMPLE_H_
#define ZIC_WAVE_SAMPLE_H_

#include "zic_file_readAudio.h"
#include "zic_wave_base.h"

class Zic_Wave_Sample : public Zic_Wave_Base {
protected:
    Zic_File_ReadAudio* file;

    int16_t sample(uint32_t* _freq)
    {
        int16_t bit;
        file.read(&bit, sizeof(bit)); // sizeof(int16_t)
        // return bit;
        return 0;
    }

public:
    Zic_Wave_Sample(Zic_File_ReadAudio* _file)
        : file(_file)
    {
    }
};

#endif
