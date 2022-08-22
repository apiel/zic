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

void debug_phdr(struct sf2_hydra_phdr* i)
{
    printf("\ndebug_phdr ----------\n");
    printf("presetName: %s\n", i->presetName);
    printf("preset: %d\n", i->preset);
    printf("bank: %d\n", i->bank);
    printf("presetBagNdx: %d\n", i->presetBagNdx);
    printf("library: %d\n", i->library);
    printf("genre: %d\n", i->genre);
    printf("morphology: %d\n", i->morphology);
}

static void sf2_hydra_read_phdr(struct sf2_hydra_phdr* i, Zic_File* file)
{
    // file->read(&i->presetName, sizeof(i->presetName));
    // file->read(&i->preset, sizeof(i->preset));
    // file->read(&i->bank, sizeof(i->bank));
    // file->read(&i->presetBagNdx, sizeof(i->presetBagNdx));
    // file->read(&i->library, sizeof(i->library));
    // file->read(&i->genre, sizeof(i->genre));
    // file->read(&i->morphology, sizeof(i->morphology));

    sf2_hydra_phdr phdr;
    file->read((uint8_t*)&phdr, sizeof(sf2_hydra_phdr) - 2);
    memcpy(i, &phdr, sizeof(sf2_hydra_phdr));

    // file->read((uint8_t*)i, 38);

    // debug_phdr(i);
}

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

struct sf2_hydra {
    struct sf2_hydra_phdr* phdrs;
    struct sf2_hydra_pbag* pbags;
    struct sf2_hydra_pmod* pmods;
    struct sf2_hydra_pgen* pgens;
    struct sf2_hydra_inst* insts;
    struct sf2_hydra_ibag* ibags;
    struct sf2_hydra_imod* imods;
    struct sf2_hydra_igen* igens;
    struct sf2_hydra_shdr* shdrs;
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

static void sf2_hydra_read_pbag(struct sf2_hydra_pbag* i, Zic_File* file)
{
    // printf("pbag size %d\n", sizeof(struct sf2_hydra_pbag));
    file->read(&i->genNdx, sizeof(i->genNdx));
    file->read(&i->modNdx, sizeof(i->modNdx));
}
static void sf2_hydra_read_pmod(struct sf2_hydra_pmod* i, Zic_File* file)
{
    file->read(&i->modSrcOper, sizeof(i->modSrcOper));
    file->read(&i->modDestOper, sizeof(i->modDestOper));
    file->read(&i->modAmount, sizeof(i->modAmount));
    file->read(&i->modAmtSrcOper, sizeof(i->modAmtSrcOper));
    file->read(&i->modTransOper, sizeof(i->modTransOper));
}
static void sf2_hydra_read_pgen(struct sf2_hydra_pgen* i, Zic_File* file)
{
    file->read(&i->genOper, sizeof(i->genOper));
    file->read(&i->genAmount, sizeof(i->genAmount));
}
static void sf2_hydra_read_inst(struct sf2_hydra_inst* i, Zic_File* file)
{
    file->read(&i->instName, sizeof(i->instName));
    file->read(&i->instBagNdx, sizeof(i->instBagNdx));
}
static void sf2_hydra_read_ibag(struct sf2_hydra_ibag* i, Zic_File* file)
{
    file->read(&i->instGenNdx, sizeof(i->instGenNdx));
    file->read(&i->instModNdx, sizeof(i->instModNdx));
}
static void sf2_hydra_read_imod(struct sf2_hydra_imod* i, Zic_File* file)
{
    file->read(&i->modSrcOper, sizeof(i->modSrcOper));
    file->read(&i->modDestOper, sizeof(i->modDestOper));
    file->read(&i->modAmount, sizeof(i->modAmount));
    file->read(&i->modAmtSrcOper, sizeof(i->modAmtSrcOper));
    file->read(&i->modTransOper, sizeof(i->modTransOper));
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

void sf2_load(Zic_File* file)
{
    struct sf2_riffchunk chunkHead;
    struct sf2_riffchunk chunkList;
    struct sf2_hydra hydra;
    float* fontSamples = NULL;
    unsigned int fontSampleCount = 0;

    if (!sf2_riffchunk_read(NULL, &chunkHead, file) || chunkHead.id != *(uint32_t*)"sfbk") {
        // if (e) *e = sf2_INVALID_NOSF2HEADER;
        return;
    }

    // Read hydra and locate sample data.
    memset(&hydra, 0, sizeof(hydra));
    while (sf2_riffchunk_read(&chunkHead, &chunkList, file)) {
        struct sf2_riffchunk chunk;
        if (chunkList.id == *(uint32_t*)"pdta") {
            while (sf2_riffchunk_read(&chunkList, &chunk, file)) {
                if (chunk.id == *(uint32_t*)"phdr" && !(chunk.size % sf2Size[phdrIdx])) {
                    sf2Offset[phdrIdx] = file->tell();
                    sf2Num[phdrIdx] = chunk.size / sf2Size[phdrIdx];
                    hydra.phdrs = (struct sf2_hydra_phdr*)malloc(sf2Num[phdrIdx] * sizeof(struct sf2_hydra_phdr));
                    if (!hydra.phdrs)
                        goto out_of_memory;
                    for (uint32_t i = 0; i < sf2Num[phdrIdx]; ++i) {
                        sf2_hydra_read_phdr(&hydra.phdrs[i], file);
                        // debug_phdr(&hydra.phdrs[i]);
                    }
                    printf("chunkName phdr (%d)\n", sf2Num[phdrIdx]);
                } else if (chunk.id == *(uint32_t*)"pbag" && !(chunk.size % sf2Size[pbagIdx])) {
                    sf2Offset[pbagIdx] = file->tell();
                    sf2Num[pbagIdx] = chunk.size / sf2Size[pbagIdx];
                    hydra.pbags = (struct sf2_hydra_pbag*)malloc(sf2Num[pbagIdx] * sizeof(struct sf2_hydra_pbag));
                    if (!hydra.pbags)
                        goto out_of_memory;
                    for (uint32_t i = 0; i < sf2Num[pbagIdx]; ++i)
                        sf2_hydra_read_pbag(&hydra.pbags[i], file);
                    printf("chunkName pbag (%d)\n", sf2Num[pbagIdx]);
                } else if (chunk.id == *(uint32_t*)"pmod" && !(chunk.size % sf2Size[pmodIdx])) {
                    sf2Offset[pmodIdx] = file->tell();
                    sf2Num[pmodIdx] = chunk.size / sf2Size[pmodIdx];
                    hydra.pmods = (struct sf2_hydra_pmod*)malloc(sf2Num[pmodIdx] * sizeof(struct sf2_hydra_pmod));
                    if (!hydra.pmods)
                        goto out_of_memory;
                    for (uint32_t i = 0; i < sf2Num[pmodIdx]; ++i)
                        sf2_hydra_read_pmod(&hydra.pmods[i], file);
                    printf("chunkName pmod (%d)\n", sf2Num[pmodIdx]);
                } else if (chunk.id == *(uint32_t*)"pgen" && !(chunk.size % sf2Size[pgenIdx])) {
                    sf2Offset[pgenIdx] = file->tell();
                    sf2Num[pgenIdx] = chunk.size / sf2Size[pgenIdx];
                    hydra.pgens = (struct sf2_hydra_pgen*)malloc(sf2Num[pgenIdx] * sizeof(struct sf2_hydra_pgen));
                    if (!hydra.pgens)
                        goto out_of_memory;
                    for (uint32_t i = 0; i < sf2Num[pgenIdx]; ++i)
                        sf2_hydra_read_pgen(&hydra.pgens[i], file);
                    printf("chunkName pgen (%d)\n", sf2Num[pgenIdx]);
                } else if (chunk.id == *(uint32_t*)"inst" && !(chunk.size % sf2Size[instIdx])) {
                    sf2Offset[instIdx] = file->tell();
                    sf2Num[instIdx] = chunk.size / sf2Size[instIdx];
                    hydra.insts = (struct sf2_hydra_inst*)malloc(sf2Num[instIdx] * sizeof(struct sf2_hydra_inst));
                    if (!hydra.insts)
                        goto out_of_memory;
                    for (uint32_t i = 0; i < sf2Num[instIdx]; ++i) {
                        sf2_hydra_read_inst(&hydra.insts[i], file);
                        // printf("inst (%d) %s\n", i, hydra.insts[i].instName);
                    }
                    printf("chunkName inst (%d)\n", sf2Num[instIdx]);
                } else if (chunk.id == *(uint32_t*)"ibag" && !(chunk.size % sf2Size[ibagIdx])) {
                    printf("ibag start %ld\n", file->tell());
                    sf2Offset[ibagIdx] = file->tell();
                    sf2Num[ibagIdx] = chunk.size / sf2Size[ibagIdx];
                    
                    hydra.ibags = (struct sf2_hydra_ibag*)malloc(sf2Num[ibagIdx] * sizeof(struct sf2_hydra_ibag));
                    if (!hydra.ibags)
                        goto out_of_memory;
                    for (uint32_t i = 0; i < sf2Num[ibagIdx]; ++i)
                        sf2_hydra_read_ibag(&hydra.ibags[i], file);
                    printf("chunkName ibag (%d)\n", sf2Num[ibagIdx]);
                } else if (chunk.id == *(uint32_t*)"imod" && !(chunk.size % sf2Size[imodIdx])) {
                    sf2Offset[imodIdx] = file->tell();
                    sf2Num[imodIdx] = chunk.size / sf2Size[imodIdx];
                    hydra.imods = (struct sf2_hydra_imod*)malloc(sf2Num[imodIdx] * sizeof(struct sf2_hydra_imod));
                    if (!hydra.imods)
                        goto out_of_memory;
                    for (uint32_t i = 0; i < sf2Num[imodIdx]; ++i)
                        sf2_hydra_read_imod(&hydra.imods[i], file);
                    printf("chunkName imod (%d)\n", sf2Num[imodIdx]);
                } else if (chunk.id == *(uint32_t*)"igen" && !(chunk.size % sf2Size[igenIdx])) {
                    sf2Offset[igenIdx] = file->tell();
                    sf2Num[igenIdx] = chunk.size / sf2Size[igenIdx];
                    file->seekFromCurrent(chunk.size);
                } else if (chunk.id == *(uint32_t*)"shdr" && !(chunk.size % sf2Size[shdrIdx])) {
                    sf2Offset[shdrIdx] = file->tell();
                    sf2Num[shdrIdx] = chunk.size / sf2Size[shdrIdx];
                    file->seekFromCurrent(chunk.size);
                } else {
                    file->seekFromCurrent(chunk.size);
                }
            }

        } else if (chunkList.id == *(uint32_t*)"sdta") {
            while (sf2_riffchunk_read(&chunkList, &chunk, file)) {
                if (chunk.id == *(uint32_t*)"smpl" && !fontSamples && chunk.size >= sizeof(short)) {
                    printf("smpl: %d\n", chunk.size);
                    if (!sf2_load_samples(&fontSamples, &fontSampleCount, &chunk, file))
                        goto out_of_memory;
                } else
                    file->seekFromCurrent(chunk.size);
            }
        } else
            file->seekFromCurrent(chunkList.size);
    }
    // if (!hydra.phdrs || !hydra.pbags || !hydra.pmods || !hydra.pgens || !hydra.insts || !hydra.ibags || !hydra.imods || !hydra.igens || !hydra.shdrs) {
    //     // if (e) *e = sf2_INVALID_INCOMPLETE;
    //     printf("sf2_INVALID_INCOMPLETE\n");
    // } else
    if (fontSamples == NULL) {
        // if (e) *e = sf2_INVALID_NOSAMPLEDATA;
        printf("sf2_INVALID_NOSAMPLEDATA\n");
    } else {
        // if (!sf2_load_presets(res, &hydra, fontSampleCount))
        //     goto out_of_memory;
        // res->fontSamples = fontSamples;
        // fontSamples = NULL; // don't free below
        // res->outSampleRate = 44100.0f;
        printf("should now load presets\n");
        // for (int i = 0; i < hydra.phdrNum; i++) {
        //     // printf("preset name %s\n", hydra.phdrs[i].presetName);
        //     debug_phdr(&hydra.phdrs[i]);
        // }
        printf("num presets %d pbagNum %d\n", sf2Num[phdrIdx], sf2Num[pbagIdx]);

        for (uint32_t i = 0; i < sf2Num[instIdx]; i++) {
            printf("----\ninstrument name %s (ibag %d)\n", hydra.insts[i].instName, hydra.insts[i].instBagNdx);

            if (hydra.insts[i].instName[0] == 'E' && hydra.insts[i].instName[1] == 'O'
                && hydra.insts[i].instName[2] == 'I' && hydra.insts[i].instName[3] == '\0') {
                printf("EOI\n");
            } else {
                printf("start %d end %d\n", hydra.insts[i].instBagNdx, hydra.insts[i + 1].instBagNdx);
                for (int j = hydra.insts[i].instBagNdx; j < hydra.insts[i + 1].instBagNdx; j++) {
                    printf("ibag (%d) %d \n", j, hydra.ibags[j].instGenNdx);

                    uint16_t g = getInstGenNdx(file, j);
                    file->seekFromStart(sf2Offset[igenIdx] + g * sf2Size[igenIdx]);
                    for (; g < sf2Num[igenIdx] && g < hydra.ibags[j + 1].instGenNdx; g++) {
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

        // printf("\n\n sample:\n");
        // for(int i = 0; i < hydra.shdrNum; i++) {
        //     printf("(%d) sample name %s\n", i, hydra.shdrs[i].sampleName);
        // }
    }
    if (0) {
    out_of_memory:
        printf("out of memory\n");
        return;
    }
    free(hydra.phdrs);
    free(hydra.pbags);
    free(hydra.pmods);
    free(hydra.pgens);
    free(hydra.insts);
    free(hydra.ibags);
    free(hydra.imods);
    free(hydra.igens);
    free(hydra.shdrs);
    free(fontSamples);
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
