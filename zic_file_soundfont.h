#ifndef ZIC_FILE_SOUNDFONT_H_
#define ZIC_FILE_SOUNDFONT_H_

#include "zic_file.h"

/**
 * Still have to get it working within zic_file_audio and zic_wave_file.
 * Even if we are able (and might still buggy) to load the sf2 file, with
 * the positions of the samples for the given preset, we still need to pich 
 * the samples depending on the pitch of the note...
 * Should picthing the note be done on the fly, or should it be done while
 * loading the preset (in memory or in a temp file)?
 * If using the idea of a temp file, we could think about a new file format
 * containing one sample per key!
 */

class Zic_File_Soundfont : public Zic_File {
protected:
    enum {
        phdrIdx = 0,
        pbagIdx,
        pmodIdx,
        pgenIdx,
        instIdx,
        ibagIdx,
        imodIdx,
        igenIdx,
        shdrIdx,
        sf2IdxCount,
    };

    uint32_t sf2Id[sf2IdxCount] = {
        *(uint32_t*)"phdr",
        *(uint32_t*)"pbag",
        *(uint32_t*)"pmod",
        *(uint32_t*)"pgen",
        *(uint32_t*)"inst",
        *(uint32_t*)"ibag",
        *(uint32_t*)"imod",
        *(uint32_t*)"igen",
        *(uint32_t*)"shdr",
    };
    uint32_t sf2Size[sf2IdxCount] = { 38, 4, 10, 4, 22, 4, 10, 4, 46 }; // cannot use sizeof struct because of memory padding https://www.javatpoint.com/structure-padding-in-c
    uint32_t sf2Num[sf2IdxCount];
    uint64_t sf2Offset[sf2IdxCount];
    uint64_t sf2SampleStart = 0;
    bool sf2Loaded = false;

    // see https://github.com/schellingb/TinySoundFont
    union sf2_genamount {
        struct {
            uint8_t lo, hi;
        } range;
        int16_t shortAmount;
        uint16_t wordAmount;
    };
    struct sf2_phdr {
        char presetName[20];
        uint16_t preset, bank, presetBagNdx;
        uint32_t library, genre, morphology;
    };

    struct sf2_pbag {
        uint16_t genNdx, modNdx;
    };
    struct sf2_pmod {
        uint16_t modSrcOper, modDestOper;
        int16_t modAmount;
        uint16_t modAmtSrcOper, modTransOper;
    };
    struct sf2_pgen {
        uint16_t genOper;
        union sf2_genamount genAmount;
    };
    struct sf2_inst {
        char instName[20];
        uint16_t instBagNdx;
    };
    struct sf2_ibag {
        uint16_t instGenNdx, instModNdx;
    };
    struct sf2_imod {
        uint16_t modSrcOper, modDestOper;
        int16_t modAmount;
        uint16_t modAmtSrcOper, modTransOper;
    };
    struct sf2_igen {
        uint16_t genOper;
        union sf2_genamount genAmount;
    };
    struct sf2_shdr {
        char sampleName[20];
        uint32_t start, end, startLoop, endLoop, sampleRate;
        uint8_t originalPitch;
        int8_t pitchCorrection;
        uint16_t sampleLink, sampleType;
    };

    struct sf2_riffchunk {
        uint32_t id;
        uint32_t size;
    };

    // see if there is a way to simplify this
    bool sf2RiffchunkRead(struct sf2_riffchunk* parent, struct sf2_riffchunk* chunk)
    {
        bool IsRiff, IsList;
        if (parent && sizeof(uint32_t) + sizeof(uint32_t) > parent->size)
            return false;
        if (!read(&chunk->id, sizeof(uint32_t)) || (char)(chunk->id & 0xFF) <= ' ' || (char)(chunk->id & 0xFF) >= 'z')
            return false;
        if (!read(&chunk->size, sizeof(uint32_t)))
            return false;
        if (parent && sizeof(uint32_t) + sizeof(uint32_t) + chunk->size > parent->size)
            return false;
        if (parent)
            parent->size -= sizeof(uint32_t) + sizeof(uint32_t) + chunk->size;
        IsRiff = chunk->id == *(uint32_t*)"RIFF", IsList = chunk->id == *(uint32_t*)"LIST";
        if (IsRiff && parent)
            return false; // not allowed
        if (!IsRiff && !IsList)
            return true; // custom type without sub type
        if (!read(&chunk->id, sizeof(uint32_t)) || (char)(chunk->id & 0xFF) <= ' ' || (char)(chunk->id & 0xFF) >= 'z')
            return false;
        chunk->size -= sizeof(uint32_t);
        return true;
    }

    uint16_t getSf2InstGenNdx(uint16_t instBagNdx)
    {
        sf2_ibag ibag;
        seekFromStart(sf2Offset[ibagIdx] + instBagNdx * sf2Size[ibagIdx]);
        read((uint8_t*)&ibag, sf2Size[ibagIdx]);
        return ibag.instGenNdx;
    }

    uint16_t getSf2PresetGenNdx(uint16_t presetBagNdx)
    {
        sf2_pbag pbag;
        seekFromStart(sf2Offset[pbagIdx] + presetBagNdx * sf2Size[pbagIdx]);
        read((uint8_t*)&pbag, sf2Size[pbagIdx]);
        return pbag.genNdx;
    }

    void getSf2Sample(uint8_t index, sf2_shdr* shdr)
    {
        seekFromStart(sf2Offset[shdrIdx] + index * sf2Size[shdrIdx]);
        read((uint8_t*)shdr, sf2Size[shdrIdx]);
    }

    void assignSf2Instrument(uint8_t index)
    {
        sf2_inst inst;
        seekFromStart(sf2Offset[instIdx] + index * sf2Size[instIdx]);
        read((uint8_t*)&inst, sf2Size[instIdx]);
        // printf("----\ninstrument name %s (ibag %d)\n", inst.instName, inst.instBagNdx);

        if (*(uint32_t*)inst.instName != *(uint32_t*)"EOI\0") {
            sf2_inst instNext;
            read((uint8_t*)&instNext, sf2Size[instIdx]);
            // printf("start %d end %d\n", inst.instBagNdx, instNext.instBagNdx);
            for (int j = inst.instBagNdx; j < instNext.instBagNdx; j++) {
                uint16_t g = getSf2InstGenNdx(j);
                seekFromStart(sf2Offset[igenIdx] + g * sf2Size[igenIdx]);
                sf2_shdr shdr;
                bool sampleFound = false;
                uint8_t lowKey = 0, highKey = 127;
                for (; g < sf2Num[igenIdx]; g++) {
                    sf2_igen igen;
                    read((uint8_t*)&igen, sf2Size[igenIdx]);
                    if (igen.genOper == 43) {
                        lowKey = igen.genAmount.range.lo;
                        highKey = igen.genAmount.range.hi;
                    } else if (igen.genOper == 53) {
                        getSf2Sample(igen.genAmount.range.lo, &shdr);
                        // printf("sample name %s\n", shdr.sampleName);
                        sampleFound = true;
                    }
                }

                if (sampleFound) {
                    for (uint8_t key = lowKey; key <= highKey; key++) {
                        memcpy(&preset.keys[key].sample, &shdr, sizeof(sf2_shdr));
                    }
                }
            }
        }
    }

    int8_t getSf2ChunkIdIndex(uint32_t chunkId)
    {
        for (uint8_t i = 0; i < sf2IdxCount; i++) {
            if (chunkId == sf2Id[i]) {
                return i;
            }
        }
        return -1;
    }

    void sf2Load()
    {
        struct sf2_riffchunk chunkHead;
        struct sf2_riffchunk chunkList;

        if (!sf2RiffchunkRead(NULL, &chunkHead) || chunkHead.id != *(uint32_t*)"sfbk") {
            return;
        }

        while (sf2RiffchunkRead(&chunkHead, &chunkList)) {
            struct sf2_riffchunk chunk;
            if (chunkList.id == *(uint32_t*)"pdta") {
                while (sf2RiffchunkRead(&chunkList, &chunk)) {

                    uint8_t index = getSf2ChunkIdIndex(chunk.id);
                    if (index != -1) {
                        sf2Offset[index] = tell();
                        sf2Num[index] = chunk.size / sf2Size[index];
                    }
                    seekFromCurrent(chunk.size);
                }
            } else if (chunkList.id == *(uint32_t*)"sdta") {
                while (sf2RiffchunkRead(&chunkList, &chunk)) {
                    if (chunk.id == *(uint32_t*)"smpl" && chunk.size >= sizeof(short)) {
                        sf2SampleStart = tell();
                    }
                    seekFromCurrent(chunk.size);
                }
            } else {
                seekFromCurrent(chunkList.size);
            }
        }
        sf2Loaded = sf2Offset[phdrIdx] && sf2Offset[pbagIdx] && sf2Offset[pmodIdx] && sf2Offset[pgenIdx]
            && sf2Offset[instIdx] && sf2Offset[ibagIdx] && sf2Offset[imodIdx] && sf2Offset[igenIdx] && sf2Offset[shdrIdx];
        // if (!sf2Loaded) {
        //     printf("sf2_INVALID_INCOMPLETE\n");
        // } else {
        //     for (uint32_t i = 0; i < sf2Num[phdrIdx]; i++) {
        //         sf2_preset* p = getPreset(i);
        //         if (p) {
        //             printf("preset name %s\n", p->phdr.presetName);
        //             printf("sample rate %d start %d end %d\n",
        //                 p->keys[0].sample.sampleRate, p->keys[0].sample.start, p->keys[0].sample.end);
        //         }
        //     }
        // }
    }

public:
    struct sf2_key {
        sf2_shdr sample;
    };

    struct sf2_preset {
        sf2_phdr phdr;
        sf2_key keys[128];
    } preset;

    Zic_File_Soundfont()
    {
    }

    Zic_File_Soundfont(const char* filename)
    {
        open(filename);
    }

    void* open(const char* filename)
    {
        if (Zic_File::open(filename, "rb") == NULL) {
            return NULL;
        }

        sf2Load();

        return file;
    }

    uint32_t getSf2PresetCount()
    {
        return sf2Num[phdrIdx];
    }

    sf2_preset* getSf2Preset(uint8_t index)
    {
        seekFromStart(sf2Offset[phdrIdx] + index * sf2Size[phdrIdx]);
        read((uint8_t*)&preset.phdr, sf2Size[phdrIdx]);
        // printf("+++++++++++\npreset name %s (pbag %d)\n", preset.phdr.presetName, preset.phdr.presetBagNdx);

        if (*(uint32_t*)preset.phdr.presetName != *(uint32_t*)"EOP\0") {
            sf2_phdr phdrNext;
            read((uint8_t*)&phdrNext, sf2Size[phdrIdx]);
            // printf("preset start %d end %d\n", preset.phdr.presetBagNdx, phdrNext.presetBagNdx);
            for (int j = preset.phdr.presetBagNdx; j < phdrNext.presetBagNdx; j++) {
                uint16_t g = getSf2PresetGenNdx(j);
                seekFromStart(sf2Offset[pgenIdx] + g * sf2Size[pgenIdx]);
                for (; g < sf2Num[pgenIdx]; g++) {
                    sf2_pgen pgen;
                    read((uint8_t*)&pgen, sf2Size[pgenIdx]);

                    if (pgen.genOper == 41) {
                        assignSf2Instrument(pgen.genAmount.range.lo);
                    } else if (pgen.genOper == 43) {
                        // see line 745 of tsf.h in TinySoundFont
                    }
                }
            }
            return &preset;
        }
        return NULL;
    }
};

#endif
