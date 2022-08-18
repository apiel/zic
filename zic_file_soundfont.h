#ifndef ZIC_FILE_SOUNDFONT_H_
#define ZIC_FILE_SOUNDFONT_H_

#include "zic_file.h"

typedef char tsf_fourcc[4];
typedef signed char tsf_s8;
typedef unsigned char tsf_u8;
typedef unsigned short tsf_u16;
typedef signed short tsf_s16;
typedef unsigned int tsf_u32;
typedef char tsf_char20[20];

union tsf_hydra_genamount {
    struct {
        tsf_u8 lo, hi;
    } range;
    tsf_s16 shortAmount;
    tsf_u16 wordAmount;
};
struct tsf_hydra_phdr {
    tsf_char20 presetName;
    tsf_u16 preset, bank, presetBagNdx;
    tsf_u32 library, genre, morphology;
};
struct tsf_hydra_pbag {
    tsf_u16 genNdx, modNdx;
};
struct tsf_hydra_pmod {
    tsf_u16 modSrcOper, modDestOper;
    tsf_s16 modAmount;
    tsf_u16 modAmtSrcOper, modTransOper;
};
struct tsf_hydra_pgen {
    tsf_u16 genOper;
    union tsf_hydra_genamount genAmount;
};
struct tsf_hydra_inst {
    tsf_char20 instName;
    tsf_u16 instBagNdx;
};
struct tsf_hydra_ibag {
    tsf_u16 instGenNdx, instModNdx;
};
struct tsf_hydra_imod {
    tsf_u16 modSrcOper, modDestOper;
    tsf_s16 modAmount;
    tsf_u16 modAmtSrcOper, modTransOper;
};
struct tsf_hydra_igen {
    tsf_u16 genOper;
    union tsf_hydra_genamount genAmount;
};
struct tsf_hydra_shdr {
    tsf_char20 sampleName;
    tsf_u32 start, end, startLoop, endLoop, sampleRate;
    tsf_u8 originalPitch;
    tsf_s8 pitchCorrection;
    tsf_u16 sampleLink, sampleType;
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
    tsf_fourcc id;
    tsf_u32 size;
};

#define TSF_TRUE 1
#define TSF_FALSE 0
#define TSF_BOOL char

#define TSF_FourCCEquals(value1, value2) (value1[0] == value2[0] && value1[1] == value2[1] && value1[2] == value2[2] && value1[3] == value2[3])

static TSF_BOOL tsf_riffchunk_read(struct tsf_riffchunk* parent, struct tsf_riffchunk* chunk, Zic_File* file)
{
    TSF_BOOL IsRiff, IsList;
    if (parent && sizeof(tsf_fourcc) + sizeof(tsf_u32) > parent->size)
        return TSF_FALSE;
    if (!file->read(&chunk->id, sizeof(tsf_fourcc)) || *chunk->id <= ' ' || *chunk->id >= 'z')
        return TSF_FALSE;
    if (!file->read(&chunk->size, sizeof(tsf_u32)))
        return TSF_FALSE;
    if (parent && sizeof(tsf_fourcc) + sizeof(tsf_u32) + chunk->size > parent->size)
        return TSF_FALSE;
    if (parent)
        parent->size -= sizeof(tsf_fourcc) + sizeof(tsf_u32) + chunk->size;
    IsRiff = TSF_FourCCEquals(chunk->id, "RIFF"), IsList = TSF_FourCCEquals(chunk->id, "LIST");
    if (IsRiff && parent)
        return TSF_FALSE; // not allowed
    if (!IsRiff && !IsList)
        return TSF_TRUE; // custom type without sub type
    if (!file->read(&chunk->id, sizeof(tsf_fourcc)) || *chunk->id <= ' ' || *chunk->id >= 'z')
        return TSF_FALSE;
    chunk->size -= sizeof(tsf_fourcc);
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

#define TSFR(FIELD) file->read(&i->FIELD, sizeof(i->FIELD));
static void tsf_hydra_read_phdr(struct tsf_hydra_phdr* i, Zic_File* file) { TSFR(presetName) TSFR(preset) TSFR(bank) TSFR(presetBagNdx) TSFR(library) TSFR(genre) TSFR(morphology) }
static void tsf_hydra_read_pbag(struct tsf_hydra_pbag* i, Zic_File* file) { TSFR(genNdx) TSFR(modNdx) }
static void tsf_hydra_read_pmod(struct tsf_hydra_pmod* i, Zic_File* file) { TSFR(modSrcOper) TSFR(modDestOper) TSFR(modAmount) TSFR(modAmtSrcOper) TSFR(modTransOper) }
static void tsf_hydra_read_pgen(struct tsf_hydra_pgen* i, Zic_File* file) { TSFR(genOper) TSFR(genAmount) }
static void tsf_hydra_read_inst(struct tsf_hydra_inst* i, Zic_File* file) { TSFR(instName) TSFR(instBagNdx) }
static void tsf_hydra_read_ibag(struct tsf_hydra_ibag* i, Zic_File* file) { TSFR(instGenNdx) TSFR(instModNdx) }
static void tsf_hydra_read_imod(struct tsf_hydra_imod* i, Zic_File* file) { TSFR(modSrcOper) TSFR(modDestOper) TSFR(modAmount) TSFR(modAmtSrcOper) TSFR(modTransOper) }
static void tsf_hydra_read_igen(struct tsf_hydra_igen* i, Zic_File* file) { TSFR(genOper) TSFR(genAmount) }
static void tsf_hydra_read_shdr(struct tsf_hydra_shdr* i, Zic_File* file) { TSFR(sampleName) TSFR(start) TSFR(end) TSFR(startLoop) TSFR(endLoop) TSFR(sampleRate) TSFR(originalPitch) TSFR(pitchCorrection) TSFR(sampleLink) TSFR(sampleType) }
#undef TSFR

void tsf_load(Zic_File* file)
{
    struct tsf_riffchunk chunkHead;
    struct tsf_riffchunk chunkList;
    struct tsf_hydra hydra;
    float* fontSamples = NULL;
    unsigned int fontSampleCount = 0;

    if (!tsf_riffchunk_read(NULL, &chunkHead, file) || !TSF_FourCCEquals(chunkHead.id, "sfbk")) {
        // if (e) *e = TSF_INVALID_NOSF2HEADER;
        return;
    }

    // Read hydra and locate sample data.
    memset(&hydra, 0, sizeof(hydra));
    while (tsf_riffchunk_read(&chunkHead, &chunkList, file)) {
        struct tsf_riffchunk chunk;
        if (TSF_FourCCEquals(chunkList.id, "pdta")) {
            while (tsf_riffchunk_read(&chunkList, &chunk, file)) {

#define HandleChunk(chunkName)                                                                                      \
    (TSF_FourCCEquals(chunk.id, #chunkName) && !(chunk.size % chunkName##SizeInFile))                               \
    {                                                                                                               \
        int num = chunk.size / chunkName##SizeInFile, i;                                                            \
        hydra.chunkName##Num = num;                                                                                 \
        hydra.chunkName##s = (struct tsf_hydra_##chunkName*)malloc(num * sizeof(struct tsf_hydra_##chunkName)); \
        if (!hydra.chunkName##s)                                                                                    \
            goto out_of_memory;                                                                                     \
        for (i = 0; i < num; ++i)                                                                                   \
            tsf_hydra_read_##chunkName(&hydra.chunkName##s[i], file);                                             \
        printf("chunkName (%d)\n", num);                                                    \
    }
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
                if HandleChunk (phdr)
                    else if HandleChunk (pbag) else if HandleChunk (pmod) else if HandleChunk (pgen) else if HandleChunk (inst) else if HandleChunk (ibag) else if HandleChunk (imod) else if HandleChunk (igen) else if HandleChunk (shdr) else file->seekFromCurrent(chunk.size);
#undef HandleChunk
            }
        } else if (TSF_FourCCEquals(chunkList.id, "sdta")) {
            while (tsf_riffchunk_read(&chunkList, &chunk, file)) {
                if (TSF_FourCCEquals(chunk.id, "smpl") && !fontSamples && chunk.size >= sizeof(short)) {
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
