#ifndef ZIC_FILE_SOUNDFONT_H_
#define ZIC_FILE_SOUNDFONT_H_

#include "zic_file.h"

union sf2_hydra_genamount {
    struct {
        uint8_t lo, hi;
    } range;
    int16_t shortAmount;
    uint16_t wordAmount;
};
struct sf2_hydra_phdr {
    char presetName[20];
    uint16_t preset, bank, presetBagNdx;
    uint32_t library, genre, morphology;
};

struct sf2_hydra_pbag {
    uint16_t genNdx, modNdx;
};
struct sf2_hydra_pmod {
    uint16_t modSrcOper, modDestOper;
    int16_t modAmount;
    uint16_t modAmtSrcOper, modTransOper;
};
struct sf2_hydra_pgen {
    uint16_t genOper;
    union sf2_hydra_genamount genAmount;
};
struct sf2_hydra_inst {
    char instName[20];
    uint16_t instBagNdx;
};
struct sf2_hydra_ibag {
    uint16_t instGenNdx, instModNdx;
};
struct sf2_hydra_imod {
    uint16_t modSrcOper, modDestOper;
    int16_t modAmount;
    uint16_t modAmtSrcOper, modTransOper;
};
struct sf2_hydra_igen {
    uint16_t genOper;
    union sf2_hydra_genamount genAmount;
};
struct sf2_hydra_shdr {
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

static bool sf2_riffchunk_read(struct sf2_riffchunk* parent, struct sf2_riffchunk* chunk, Zic_File* file)
{
    bool IsRiff, IsList;
    if (parent && sizeof(uint32_t) + sizeof(uint32_t) > parent->size)
        return false;
    if (!file->read(&chunk->id, sizeof(uint32_t)) || (char)(chunk->id & 0xFF) <= ' ' || (char)(chunk->id & 0xFF) >= 'z')
        return false;
    if (!file->read(&chunk->size, sizeof(uint32_t)))
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
    if (!file->read(&chunk->id, sizeof(uint32_t)) || (char)(chunk->id & 0xFF) <= ' ' || (char)(chunk->id & 0xFF) >= 'z')
        return false;
    chunk->size -= sizeof(uint32_t);
    return true;
}

static int sf2_load_samples(float** fontSamples, unsigned int* fontSampleCount, struct sf2_riffchunk* chunkSmpl, Zic_File* file)
{
    // Read sample data into float format buffer.
    float* out;
    unsigned int samplesLeft, samplesToRead, samplesToConvert;
    samplesLeft = *fontSampleCount = chunkSmpl->size / sizeof(short);
    out = *fontSamples = (float*)malloc(samplesLeft * sizeof(float));
    if (!out)
        return 0;
    for (; samplesLeft; samplesLeft -= samplesToRead) {
        short sampleBuffer[1024], *in = sampleBuffer;
        ;
        samplesToRead = (samplesLeft > 1024 ? 1024 : samplesLeft);
        file->read(sampleBuffer, samplesToRead * sizeof(short));

        // Convert from signed 16-bit to float.
        for (samplesToConvert = samplesToRead; samplesToConvert > 0; --samplesToConvert)
            // If we ever need to compile for big-endian platforms, we'll need to byte-swap here.
            *out++ = (float)(*in++ / 32767.0);
    }
    return 1;
}

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

void printSample(Zic_File* file, uint8_t index)
{
    sf2_hydra_shdr shdr;
    file->seekFromStart(sf2Offset[shdrIdx] + index * sf2Size[shdrIdx]);
    file->read((uint8_t*)&shdr, sf2Size[shdrIdx]);
    printf("sample name %s\n", shdr.sampleName);
}

uint16_t getInstGenNdx(Zic_File* file, uint16_t instBagNdx)
{
    sf2_hydra_ibag ibag;
    file->seekFromStart(sf2Offset[ibagIdx] + instBagNdx * sf2Size[ibagIdx]);
    file->read((uint8_t*)&ibag, sf2Size[ibagIdx]);
    return ibag.instGenNdx;
}

void printInstrument(Zic_File* file, uint8_t index)
{
    sf2_hydra_inst inst;
    file->seekFromStart(sf2Offset[instIdx] + index * sf2Size[instIdx]);
    file->read((uint8_t*)&inst, sf2Size[instIdx]);
    printf("----\ninstrument name %s (ibag %d)\n", inst.instName, inst.instBagNdx);

    // maybe there is a better way to do this? with uint32?
    if (inst.instName[0] == 'E' && inst.instName[1] == 'O' && inst.instName[2] == 'I' && inst.instName[3] == '\0') {
        printf("EOI\n");
    } else {
        sf2_hydra_inst instNext;
        file->read((uint8_t*)&instNext, sf2Size[instIdx]);
        printf("start %d end %d\n", inst.instBagNdx, instNext.instBagNdx);
        for (int j = inst.instBagNdx; j < instNext.instBagNdx; j++) {
            uint16_t g = getInstGenNdx(file, j);
            file->seekFromStart(sf2Offset[igenIdx] + g * sf2Size[igenIdx]);
            for (; g < sf2Num[igenIdx]; g++) {
                sf2_hydra_igen igen;
                file->read((uint8_t*)&igen, sf2Size[igenIdx]);

                if (igen.genOper == 43 || igen.genOper == 53) {
                    printf("> igen (%d) %d hi %d low %d\tshortAmount %d wordAmount %d\n", g,
                        igen.genOper,
                        igen.genAmount.range.hi,
                        igen.genAmount.range.lo,
                        igen.genAmount.shortAmount,
                        igen.genAmount.wordAmount);
                }
                // if 43 we should assign the key range
                if (igen.genOper == 53) {
                    // here we should assign sample value
                    printSample(file, igen.genAmount.range.lo);
                }
            }
        }
    }
}

void printPreset(Zic_File* file, uint8_t index)
{
    sf2_hydra_phdr phdr;
    file->seekFromStart(sf2Offset[phdrIdx] + index * sf2Size[phdrIdx]);
    file->read((uint8_t*)&phdr, sf2Size[phdrIdx]);
    printf("preset name %s (pbag %d)\n", phdr.presetName, phdr.presetBagNdx);
}

int8_t getChunkIdIndex(uint32_t chunkId)
{
    for (uint8_t i = 0; i < sf2IdxCount; i++) {
        if (chunkId == sf2Id[i]) {
            return i;
        }
    }
    return -1;
}

void sf2_load(Zic_File* file)
{
    struct sf2_riffchunk chunkHead;
    struct sf2_riffchunk chunkList;
    float* fontSamples = NULL;
    unsigned int fontSampleCount = 0;

    if (!sf2_riffchunk_read(NULL, &chunkHead, file) || chunkHead.id != *(uint32_t*)"sfbk") {
        // if (e) *e = sf2_INVALID_NOSF2HEADER;
        return;
    }

    while (sf2_riffchunk_read(&chunkHead, &chunkList, file)) {
        struct sf2_riffchunk chunk;
        if (chunkList.id == *(uint32_t*)"pdta") {
            while (sf2_riffchunk_read(&chunkList, &chunk, file)) {

                uint8_t index = getChunkIdIndex(chunk.id);
                if (index != -1) {
                    sf2Offset[index] = file->tell();
                    sf2Num[index] = chunk.size / sf2Size[index];
                }
                file->seekFromCurrent(chunk.size);
            }
        } else if (chunkList.id == *(uint32_t*)"sdta") {
            while (sf2_riffchunk_read(&chunkList, &chunk, file)) {
                if (chunk.id == *(uint32_t*)"smpl" && !fontSamples && chunk.size >= sizeof(short)) {
                    printf("smpl: %d\n", chunk.size);
                    if (!sf2_load_samples(&fontSamples, &fontSampleCount, &chunk, file)) {
                        printf("out of memory\n");
                        return;
                    }
                } else
                    file->seekFromCurrent(chunk.size);
            }
        } else
            file->seekFromCurrent(chunkList.size);
    }
    // here we could check that each offset are set...
    // if (!hydra.phdrs || !hydra.pbags || !hydra.pmods || !hydra.pgens || !hydra.insts || !hydra.ibags || !hydra.imods || !hydra.igens || !hydra.shdrs) {
    //     // if (e) *e = sf2_INVALID_INCOMPLETE;
    //     printf("sf2_INVALID_INCOMPLETE\n");
    // } else
    if (fontSamples == NULL) {
        // if (e) *e = sf2_INVALID_NOSAMPLEDATA;
        printf("sf2_INVALID_NOSAMPLEDATA\n");
    } else {
        printf("num presets %d pbagNum %d\n", sf2Num[phdrIdx], sf2Num[pbagIdx]);

        for (uint32_t i = 0; i < sf2Num[instIdx]; i++) {
            printInstrument(file, i);
        }

        for (uint32_t i = 0; i < sf2Num[phdrIdx]; i++) {
            printPreset(file, i);
        }

        // printf("\n\n sample:\n");
        // for(int i = 0; i < hydra.shdrNum; i++) {
        //     printf("(%d) sample name %s\n", i, hydra.shdrs[i].sampleName);
        // }
    }
}

class Zic_File_Soundfont : public Zic_File {
protected:
    // https://mrtenz.github.io/soundfont2/getting-started/soundfont2-structure.html
    // https://github.com/gagern/soundfontutils/blob/master/YamlFormat.md
    // https://github.com/Mrtenz/soundfont2.git
    // http://www.synthfont.com/Tutorial6.html

public:
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

        sf2_load(this);

        // uint32_t chunkID;
        // uint32_t chunkSize;
        // while (Zic_File::read(&chunkID, 4)) {
        //     // printf("(%d) %c%c%c%c\n", chunkID, (char)(chunkID & 0xFF), (char)((chunkID >> 8) & 0xFF), (char)((chunkID >> 16) & 0xFF), (char)((chunkID >> 24) & 0xFF));
        //     if (chunkID == *(uint32_t*)"pdta") {
        //         printf("Found pdta\n");
        //         // Zic_File::read(&chunkSize, 4);
        //         // printf("pdta Data: %d\n", chunkSize);
        //         // seekFromCurrent(chunkSize);
        //     } else if (chunkID == *(uint32_t*)"sdta") {
        //         printf("Found sdta\n");
        //     } else if (chunkID == *(uint32_t*)"smpl") {
        //         printf("Found smpl\n");
        //     } else if (chunkID == *(uint32_t*)"sm24") {
        //         printf("Found sm24\n");
        //     } else if (chunkID == *(uint32_t*)"RIFF") {
        //         printf("Found RIFF\n");
        //     } else if (chunkID == *(uint32_t*)"LIST") {
        //         printf("Found LIST\n");
        //     } else if (chunkID == *(uint32_t*)"sfbk") {
        //         printf("Found sfbk\n");
        //     } else if (chunkID == *(uint32_t*)"INFO") {
        //         printf("Found INFO\n");
        //     } else if (chunkID == *(uint32_t*)"INAM") {
        //         printf("Found INAM\n");
        //     } else if (chunkID == *(uint32_t*)"shdr") {
        //         printf("Found shdr\n");
        //     } else if (chunkID == *(uint32_t*)"pbag") {
        //         printf("Found pbag\n");
        //     } else {
        //         // char * chunkName = (char*)&chunkID;
        //         // if (*chunkName > ' ' && *chunkName < 'z') {
        //         //     printf("Found something %c%c%c%c\n", *chunkName, *(chunkName + 1), *(chunkName + 2), *(chunkName + 3));
        //         // }
        //     }
        // }

        return file;
    }
};

#endif
