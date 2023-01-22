#ifndef ZIC_WAVETABLE_FILE_H_
#define ZIC_WAVETABLE_FILE_H_

#if ZIC_USE_LIBSNDFILE == 1
#include "zic_file_audio_libsndfile.h"
#else
#include "zic_file_audio_custom.h"
#endif

#include "./wavetables/wavetable_sine.h"

#ifndef ZIC_WAVETABLES_MAX_SAMPLES
#define ZIC_WAVETABLES_MAX_SAMPLES 2048
#endif

class Zic_Wavetable_File {
protected:
    uint64_t start = 0;

    void setSampleParams(uint64_t _start, uint64_t _sampleCount)
    {
        // shoud it be: if (_start <= audioFile.sampleCount) {
        // and not care if the target sampleCount is less the requested one
        if (_start + _sampleCount <= audioFile.sampleCount) {
            start = _start;
            sampleCount = _sampleCount;

            audioFile.seekToSample(start);
            for (uint64_t i = 0; i < ZIC_WAVETABLES_MAX_SAMPLES && i < sampleCount; i++) {
                table[i] = audioFile.readSampleFloat();
            }
        }
    }

public:
    float sampleIndex = 0.0f;
    uint64_t sampleCount = 0;
    float table[ZIC_WAVETABLES_MAX_SAMPLES];

    Zic_File_Audio audioFile;

    Zic_Wavetable_File()
    {
        // Load default wavetable
        sampleCount = wavetable_Sine.sampleCount;
        for (uint64_t i = 0; i < ZIC_WAVETABLES_MAX_SAMPLES && i < sampleCount; i++) {
            table[i] = wavetable_Sine.table[i] / 32768.0f;
        }
    }

    float getValueAt(uint64_t index)
    {
        return table[index];
    }

    uint64_t getSampleCount()
    {
        return sampleCount;
    }

    void open(const char* filename)
    {
        audioFile.open(filename);
        sampleCount = (float)audioFile.sampleCount / (float)audioFile.wavetableCount;

        setSampleParams(start, sampleCount);
    }

    void morphPct(float value)
    {
        value = range(value, 0.0f, 1.0f);
        morph(value * (float)audioFile.wavetableCount);
    }

    void morph(float value)
    {
        value = range(value, 0.0f, (float)audioFile.wavetableCount - 1.0f);
        setSampleParams((uint64_t)(value * sampleCount), sampleCount);
    }

    float getMorph()
    {
        return (float)start / (float)sampleCount;
    }
};

#endif
