#ifndef ZIC_FILE_SOUNDFONT_H_
#define ZIC_FILE_SOUNDFONT_H_

#include "zic_file.h"

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

        uint32_t chunkID;
        uint32_t chunkSize;
        while (Zic_File::read(&chunkID, 4)) {
            // printf("(%d) %c%c%c%c\n", chunkID, (char)(chunkID & 0xFF), (char)((chunkID >> 8) & 0xFF), (char)((chunkID >> 16) & 0xFF), (char)((chunkID >> 24) & 0xFF));
            if (chunkID == *(uint32_t*)"pdta") {
                printf("Found pdta\n");
                // Zic_File::read(&chunkSize, 4);
                // printf("pdta Data: %d\n", chunkSize);
                // seekFromCurrent(chunkSize);
            } else if (chunkID == *(uint32_t*)"sdta") {
                printf("Found sdta\n");
            } else if (chunkID == *(uint32_t*)"smpl") {
                printf("Found smpl\n");
            } else if (chunkID == *(uint32_t*)"sm24") {
                printf("Found sm24\n");
            } else if (chunkID == *(uint32_t*)"RIFF") {
                printf("Found RIFF\n");
            } else if (chunkID == *(uint32_t*)"LIST") {
                printf("Found LIST\n");
            } else if (chunkID == *(uint32_t*)"sfbk") {
                printf("Found sfbk\n");
            } else if (chunkID == *(uint32_t*)"INFO") {
                printf("Found INFO\n");
            } else if (chunkID == *(uint32_t*)"INAM") {
                printf("Found INAM\n");
            } else if (chunkID == *(uint32_t*)"shdr") {
                printf("Found shdr\n");
            } else if (chunkID == *(uint32_t*)"pbag") {
                printf("Found pbag\n");
            } else {
                // char * chunkName = (char*)&chunkID;
                // if (*chunkName > ' ' && *chunkName < 'z') {
                //     printf("Found something %c%c%c%c\n", *chunkName, *(chunkName + 1), *(chunkName + 2), *(chunkName + 3));
                // }
            }
        }

        return file;
    }
};

#endif
