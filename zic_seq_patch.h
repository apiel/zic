#ifndef ZIC_SEQ_PATCH_H_
#define ZIC_SEQ_PATCH_H_

#ifndef ZIC_PATCH_MAX_FLOATS
#define ZIC_PATCH_MAX_FLOATS 500
#endif

#ifndef ZIC_PATCH_MAX_STRINGS
#define ZIC_PATCH_MAX_STRINGS 50
#endif

#ifndef ZIC_PATCH_STRING_LENGTH
#define ZIC_PATCH_STRING_LENGTH 50
#endif

#ifndef ZIC_PATCH_MAX_CC
#define ZIC_PATCH_MAX_CC 127
#endif

#include <stdint.h>
#include <string.h>

class Zic_Seq_Patch {
public:
    uint16_t id = -1; // Set to biggest number to avoid conflicts
    float floats[ZIC_PATCH_MAX_FLOATS];
    char strings[ZIC_PATCH_MAX_STRINGS][ZIC_PATCH_STRING_LENGTH];
    // Keep it simple for the moment and not have voice/channel for cc
    uint8_t cc[ZIC_PATCH_MAX_CC];

    uint16_t floatsCount = 0;
    uint16_t stringsCount = 0;
    uint16_t ccCount = 0;

    void set(Zic_Seq_Patch* patch)
    {
        id = patch->id;
        
        floatsCount = patch->floatsCount;
        stringsCount = patch->stringsCount;
        ccCount = patch->ccCount;
        for (uint16_t i = 0; i < floatsCount; i++) {
            floats[i] = patch->floats[i];
        }
        for (uint16_t i = 0; i < stringsCount; i++) {
            strcpy(strings[i], patch->strings[i]);
        }
        for (uint16_t i = 0; i < ccCount; i++) {
            cc[i] = patch->cc[i];
        }
    }

    void setCc(uint8_t index, uint8_t value)
    {
        if (index < ZIC_PATCH_MAX_CC) {
            cc[index] = value;
            if (index >= ccCount) {
                ccCount = index + 1;
            }
        }
    }

    void setFloat(uint16_t index, float value)
    {
        if (index < ZIC_PATCH_MAX_FLOATS) {
            floats[index] = value;
            if (index >= floatsCount) {
                floatsCount = index + 1;
            }
        }
    }

    void setString(uint16_t index, const char* value)
    {
        if (index < ZIC_PATCH_MAX_STRINGS) {
            strncpy(strings[index], value, ZIC_PATCH_STRING_LENGTH);
            if (index >= stringsCount) {
                stringsCount = index + 1;
            }
        }
    }

    float getFloat(uint16_t index)
    {
        if (index < floatsCount) {
            return floats[index];
        }
        return 0;
    }

    char* getString(uint16_t index)
    {
        if (index < stringsCount) {
            return strings[index];
        }
        return (char*)"";
    }

    void clear()
    {
        for (uint16_t i = 0; i < floatsCount; i++) {
            floats[i] = 0.0f;
        }
        for (uint16_t i = 0; i < stringsCount; i++) {
            strcpy(strings[i], "");
        }
        for (uint16_t i = 0; i < ccCount; i++) {
            cc[i] = 0;
        }
        floatsCount = 0;
        stringsCount = 0;
        ccCount = 0;
    }
};

#endif