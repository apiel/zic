#ifndef ZIC_FILE_SOUNDFONT_H_
#define ZIC_FILE_SOUNDFONT_H_

#include "zic_file.h"

union tsf_hydra_genamount {
    struct {
        uint8_t lo, hi;
    } range;
    int16_t shortAmount;
    uint16_t wordAmount;
};
struct tsf_hydra_phdr {
    char presetName[20];
    uint16_t preset, bank, presetBagNdx;
    uint32_t library, genre, morphology;
};

void debug_phdr(struct tsf_hydra_phdr* i)
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

static void tsf_hydra_read_phdr(struct tsf_hydra_phdr* i, Zic_File* file)
{
    file->read(&i->presetName, sizeof(i->presetName));
    file->read(&i->preset, sizeof(i->preset));
    file->read(&i->bank, sizeof(i->bank));
    file->read(&i->presetBagNdx, sizeof(i->presetBagNdx));
    file->read(&i->library, sizeof(i->library));
    file->read(&i->genre, sizeof(i->genre));
    file->read(&i->morphology, sizeof(i->morphology));

    // tsf_hydra_phdr phdr;
    // file->read((uint8_t*)&phdr, sizeof(tsf_hydra_phdr));
    // memcpy(i, &phdr, sizeof(tsf_hydra_phdr));
    // debug_phdr(&phdr);

    // file->read((uint8_t*)i, sizeof(tsf_hydra_phdr));
}

struct tsf_hydra_pbag {
    uint16_t genNdx, modNdx;
};
struct tsf_hydra_pmod {
    uint16_t modSrcOper, modDestOper;
    int16_t modAmount;
    uint16_t modAmtSrcOper, modTransOper;
};
struct tsf_hydra_pgen {
    uint16_t genOper;
    union tsf_hydra_genamount genAmount;
};
struct tsf_hydra_inst {
    char instName[20];
    uint16_t instBagNdx;
};
struct tsf_hydra_ibag {
    uint16_t instGenNdx, instModNdx;
};
struct tsf_hydra_imod {
    uint16_t modSrcOper, modDestOper;
    int16_t modAmount;
    uint16_t modAmtSrcOper, modTransOper;
};
struct tsf_hydra_igen {
    uint16_t genOper;
    union tsf_hydra_genamount genAmount;
};
struct tsf_hydra_shdr {
    char sampleName[20];
    uint32_t start, end, startLoop, endLoop, sampleRate;
    uint8_t originalPitch;
    int8_t pitchCorrection;
    uint16_t sampleLink, sampleType;
};

struct tsf_hydra {
    struct tsf_hydra_phdr* phdrs;
    struct tsf_hydra_pbag* pbags;
    struct tsf_hydra_pmod* pmods;
    struct tsf_hydra_pgen* pgens;
    struct tsf_hydra_inst* insts;
    struct tsf_hydra_ibag* ibags;
    struct tsf_hydra_imod* imods;
    struct tsf_hydra_igen* igens;
    struct tsf_hydra_shdr* shdrs;
    int phdrNum, pbagNum, pmodNum, pgenNum, instNum, ibagNum, imodNum, igenNum, shdrNum;
};

struct tsf_riffchunk {
    uint32_t id;
    uint32_t size;
};

#define TSF_TRUE 1
#define TSF_FALSE 0
#define TSF_BOOL char

static TSF_BOOL tsf_riffchunk_read(struct tsf_riffchunk* parent, struct tsf_riffchunk* chunk, Zic_File* file)
{
    TSF_BOOL IsRiff, IsList;
    if (parent && sizeof(uint32_t) + sizeof(uint32_t) > parent->size)
        return TSF_FALSE;
    if (!file->read(&chunk->id, sizeof(uint32_t)) || (char)(chunk->id & 0xFF) <= ' ' || (char)(chunk->id & 0xFF) >= 'z')
        return TSF_FALSE;
    if (!file->read(&chunk->size, sizeof(uint32_t)))
        return TSF_FALSE;
    if (parent && sizeof(uint32_t) + sizeof(uint32_t) + chunk->size > parent->size)
        return TSF_FALSE;
    if (parent)
        parent->size -= sizeof(uint32_t) + sizeof(uint32_t) + chunk->size;
    IsRiff = chunk->id == *(uint32_t*)"RIFF", IsList = chunk->id == *(uint32_t*)"LIST";
    if (IsRiff && parent)
        return TSF_FALSE; // not allowed
    if (!IsRiff && !IsList)
        return TSF_TRUE; // custom type without sub type
    if (!file->read(&chunk->id, sizeof(uint32_t)) || (char)(chunk->id & 0xFF) <= ' ' || (char)(chunk->id & 0xFF) >= 'z')
        return TSF_FALSE;
    chunk->size -= sizeof(uint32_t);
    return TSF_TRUE;
}

static int tsf_load_samples(float** fontSamples, unsigned int* fontSampleCount, struct tsf_riffchunk* chunkSmpl, Zic_File* file)
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

static void tsf_hydra_read_pbag(struct tsf_hydra_pbag* i, Zic_File* file)
{
    file->read(&i->genNdx, sizeof(i->genNdx));
    file->read(&i->modNdx, sizeof(i->modNdx));
}
static void tsf_hydra_read_pmod(struct tsf_hydra_pmod* i, Zic_File* file)
{
    file->read(&i->modSrcOper, sizeof(i->modSrcOper));
    file->read(&i->modDestOper, sizeof(i->modDestOper));
    file->read(&i->modAmount, sizeof(i->modAmount));
    file->read(&i->modAmtSrcOper, sizeof(i->modAmtSrcOper));
    file->read(&i->modTransOper, sizeof(i->modTransOper));
}
static void tsf_hydra_read_pgen(struct tsf_hydra_pgen* i, Zic_File* file)
{
    file->read(&i->genOper, sizeof(i->genOper));
    file->read(&i->genAmount, sizeof(i->genAmount));
}
static void tsf_hydra_read_inst(struct tsf_hydra_inst* i, Zic_File* file)
{
    file->read(&i->instName, sizeof(i->instName));
    file->read(&i->instBagNdx, sizeof(i->instBagNdx));
}
static void tsf_hydra_read_ibag(struct tsf_hydra_ibag* i, Zic_File* file)
{
    file->read(&i->instGenNdx, sizeof(i->instGenNdx));
    file->read(&i->instModNdx, sizeof(i->instModNdx));
}
static void tsf_hydra_read_imod(struct tsf_hydra_imod* i, Zic_File* file)
{
    file->read(&i->modSrcOper, sizeof(i->modSrcOper));
    file->read(&i->modDestOper, sizeof(i->modDestOper));
    file->read(&i->modAmount, sizeof(i->modAmount));
    file->read(&i->modAmtSrcOper, sizeof(i->modAmtSrcOper));
    file->read(&i->modTransOper, sizeof(i->modTransOper));
}
static void tsf_hydra_read_igen(struct tsf_hydra_igen* i, Zic_File* file)
{
    file->read(&i->genOper, sizeof(i->genOper));
    file->read(&i->genAmount, sizeof(i->genAmount));
}
static void tsf_hydra_read_shdr(struct tsf_hydra_shdr* i, Zic_File* file)
{
    file->read(&i->sampleName, sizeof(i->sampleName));
    file->read(&i->start, sizeof(i->start));
    file->read(&i->end, sizeof(i->end));
    file->read(&i->startLoop, sizeof(i->startLoop));
    file->read(&i->endLoop, sizeof(i->endLoop));
    file->read(&i->sampleRate, sizeof(i->sampleRate));
    file->read(&i->originalPitch, sizeof(i->originalPitch));
    file->read(&i->pitchCorrection, sizeof(i->pitchCorrection));
    file->read(&i->sampleLink, sizeof(i->sampleLink));
    file->read(&i->sampleType, sizeof(i->sampleType));
}

void tsf_load(Zic_File* file)
{
    struct tsf_riffchunk chunkHead;
    struct tsf_riffchunk chunkList;
    struct tsf_hydra hydra;
    float* fontSamples = NULL;
    unsigned int fontSampleCount = 0;

    if (!tsf_riffchunk_read(NULL, &chunkHead, file) || chunkHead.id != *(uint32_t*)"sfbk") {
        // if (e) *e = TSF_INVALID_NOSF2HEADER;
        return;
    }

    // Read hydra and locate sample data.
    memset(&hydra, 0, sizeof(hydra));
    while (tsf_riffchunk_read(&chunkHead, &chunkList, file)) {
        struct tsf_riffchunk chunk;
        if (chunkList.id == *(uint32_t*)"pdta") {
            while (tsf_riffchunk_read(&chunkList, &chunk, file)) {

                enum {
                    phdrSizeInFile = 38,
                    pbagSizeInFile = 4,
                    pmodSizeInFile = 10,
                    pgenSizeInFile = 4,
                    instSizeInFile = 22,
                    ibagSizeInFile = 4,
                    imodSizeInFile = 10,
                    igenSizeInFile = 4,
                    shdrSizeInFile = 46
                };
                if (chunk.id == *(uint32_t*)"phdr" && !(chunk.size % phdrSizeInFile)) {
                    int num = chunk.size / phdrSizeInFile, i;
                    hydra.phdrNum = num;
                    hydra.phdrs = (struct tsf_hydra_phdr*)malloc(num * sizeof(struct tsf_hydra_phdr));
                    if (!hydra.phdrs)
                        goto out_of_memory;
                    for (i = 0; i < num; ++i)
                        tsf_hydra_read_phdr(&hydra.phdrs[i], file);
                    printf("chunkName (%d)\n", num);
                } else if (chunk.id == *(uint32_t*)"pbag" && !(chunk.size % pbagSizeInFile)) {
                    int num = chunk.size / pbagSizeInFile, i;
                    hydra.pbagNum = num;
                    hydra.pbags = (struct tsf_hydra_pbag*)malloc(num * sizeof(struct tsf_hydra_pbag));
                    if (!hydra.pbags)
                        goto out_of_memory;
                    for (i = 0; i < num; ++i)
                        tsf_hydra_read_pbag(&hydra.pbags[i], file);
                    printf("chunkName (%d)\n", num);
                } else if (chunk.id == *(uint32_t*)"pmod" && !(chunk.size % pmodSizeInFile)) {
                    int num = chunk.size / pmodSizeInFile, i;
                    hydra.pmodNum = num;
                    hydra.pmods = (struct tsf_hydra_pmod*)malloc(num * sizeof(struct tsf_hydra_pmod));
                    if (!hydra.pmods)
                        goto out_of_memory;
                    for (i = 0; i < num; ++i)
                        tsf_hydra_read_pmod(&hydra.pmods[i], file);
                    printf("chunkName (%d)\n", num);
                } else if (chunk.id == *(uint32_t*)"pgen" && !(chunk.size % pgenSizeInFile)) {
                    int num = chunk.size / pgenSizeInFile, i;
                    hydra.pgenNum = num;
                    hydra.pgens = (struct tsf_hydra_pgen*)malloc(num * sizeof(struct tsf_hydra_pgen));
                    if (!hydra.pgens)
                        goto out_of_memory;
                    for (i = 0; i < num; ++i)
                        tsf_hydra_read_pgen(&hydra.pgens[i], file);
                    printf("chunkName (%d)\n", num);
                } else if (chunk.id == *(uint32_t*)"inst" && !(chunk.size % instSizeInFile)) {
                    int num = chunk.size / instSizeInFile, i;
                    hydra.instNum = num;
                    hydra.insts = (struct tsf_hydra_inst*)malloc(num * sizeof(struct tsf_hydra_inst));
                    if (!hydra.insts)
                        goto out_of_memory;
                    for (i = 0; i < num; ++i)
                        tsf_hydra_read_inst(&hydra.insts[i], file);
                    printf("chunkName (%d)\n", num);
                } else if (chunk.id == *(uint32_t*)"ibag" && !(chunk.size % ibagSizeInFile)) {
                    int num = chunk.size / ibagSizeInFile, i;
                    hydra.ibagNum = num;
                    hydra.ibags = (struct tsf_hydra_ibag*)malloc(num * sizeof(struct tsf_hydra_ibag));
                    if (!hydra.ibags)
                        goto out_of_memory;
                    for (i = 0; i < num; ++i)
                        tsf_hydra_read_ibag(&hydra.ibags[i], file);
                    printf("chunkName (%d)\n", num);
                } else if (chunk.id == *(uint32_t*)"imod" && !(chunk.size % imodSizeInFile)) {
                    int num = chunk.size / imodSizeInFile, i;
                    hydra.imodNum = num;
                    hydra.imods = (struct tsf_hydra_imod*)malloc(num * sizeof(struct tsf_hydra_imod));
                    if (!hydra.imods)
                        goto out_of_memory;
                    for (i = 0; i < num; ++i)
                        tsf_hydra_read_imod(&hydra.imods[i], file);
                    printf("chunkName (%d)\n", num);
                } else if (chunk.id == *(uint32_t*)"igen" && !(chunk.size % igenSizeInFile)) {
                    int num = chunk.size / igenSizeInFile, i;
                    hydra.igenNum = num;
                    hydra.igens = (struct tsf_hydra_igen*)malloc(num * sizeof(struct tsf_hydra_igen));
                    if (!hydra.igens)
                        goto out_of_memory;
                    for (i = 0; i < num; ++i)
                        tsf_hydra_read_igen(&hydra.igens[i], file);
                    printf("chunkName (%d)\n", num);
                } else if (chunk.id == *(uint32_t*)"shdr" && !(chunk.size % shdrSizeInFile)) {
                    int num = chunk.size / shdrSizeInFile, i;
                    hydra.shdrNum = num;
                    hydra.shdrs = (struct tsf_hydra_shdr*)malloc(num * sizeof(struct tsf_hydra_shdr));
                    if (!hydra.shdrs)
                        goto out_of_memory;
                    for (i = 0; i < num; ++i)
                        tsf_hydra_read_shdr(&hydra.shdrs[i], file);
                    printf("chunkName (%d)\n", num);
                } else
                    file->seekFromCurrent(chunk.size);
            }

        } else if (chunkList.id == *(uint32_t*)"sdta") {
            while (tsf_riffchunk_read(&chunkList, &chunk, file)) {
                if (chunk.id == *(uint32_t*)"smpl" && !fontSamples && chunk.size >= sizeof(short)) {
                    printf("smpl: %d\n", chunk.size);
                    if (!tsf_load_samples(&fontSamples, &fontSampleCount, &chunk, file))
                        goto out_of_memory;
                } else
                    file->seekFromCurrent(chunk.size);
            }
        } else
            file->seekFromCurrent(chunkList.size);
    }
    if (!hydra.phdrs || !hydra.pbags || !hydra.pmods || !hydra.pgens || !hydra.insts || !hydra.ibags || !hydra.imods || !hydra.igens || !hydra.shdrs) {
        // if (e) *e = TSF_INVALID_INCOMPLETE;
        printf("TSF_INVALID_INCOMPLETE\n");
    } else if (fontSamples == NULL) {
        // if (e) *e = TSF_INVALID_NOSAMPLEDATA;
        printf("TSF_INVALID_NOSAMPLEDATA\n");
    } else {
        // if (!tsf_load_presets(res, &hydra, fontSampleCount))
        //     goto out_of_memory;
        // res->fontSamples = fontSamples;
        // fontSamples = NULL; // don't free below
        // res->outSampleRate = 44100.0f;
        printf("should now load presets\n");
        for (int i = 0; i < hydra.phdrNum; i++) {
            // printf("preset name %s\n", hydra.phdrs[i].presetName);
            debug_phdr(&hydra.phdrs[i]);
        }
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

        tsf_load(this);

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
