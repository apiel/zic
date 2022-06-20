#ifndef ZIC_NOTE_H_
#define ZIC_NOTE_H_

#include <stdint.h>

#define _NOTE_START _C0
#define _NOTE_END _B8

#define _C0 12
#define _C_0 13
#define _D0 14
#define _D_0 15
#define _E0 16
#define _F0 17
#define _F_0 18
#define _G0 19
#define _G_0 20
#define _A0 21
#define _A_0 22
#define _B0 23

#define _C1 24
#define _C_1 25
#define _D1 26
#define _D_1 27
#define _E1 28
#define _F1 29
#define _F_1 30
#define _G1 31
#define _G_1 32
#define _A1 33
#define _A_1 34
#define _B1 35

#define _C2 36
#define _C_2 37
#define _D2 38
#define _D_2 39
#define _E2 40
#define _F2 41
#define _F_2 42
#define _G2 43
#define _G_2 44
#define _A2 45
#define _A_2 46
#define _B2 47

#define _C3 48
#define _C_3 49
#define _D3 50
#define _D_3 51
#define _E3 52
#define _F3 53
#define _F_3 54
#define _G3 55
#define _G_3 56
#define _A3 57
#define _A_3 58
#define _B3 59

#define _C4 60
#define _C_4 61
#define _D4 62
#define _D_4 63
#define _E4 64
#define _F4 65
#define _F_4 66
#define _G4 67
#define _G_4 68
#define _A4 69
#define _A_4 70
#define _B4 71

#define _C5 72
#define _C_5 73
#define _D5 74
#define _D_5 75
#define _E5 76
#define _F5 77
#define _F_5 78
#define _G5 79
#define _G_5 80
#define _A5 81
#define _A_5 82
#define _B5 83

#define _C6 84
#define _C_6 85
#define _D6 86
#define _D_6 87
#define _E6 88
#define _F6 89
#define _F_6 90
#define _G6 91
#define _G_6 92
#define _A6 93
#define _A_6 94
#define _B6 95

#define _C7 96
#define _C_7 97
#define _D7 98
#define _D_7 99
#define _E7 100
#define _F7 101
#define _F_7 102
#define _G7 103
#define _G_7 104
#define _A7 105
#define _A_7 106
#define _B7 107

#define _C8 108
#define _C_8 109
#define _D8 110
#define _D_8 111
#define _E8 112
#define _F8 113
#define _F_8 114
#define _G8 115
#define _G_8 116
#define _A8 117
#define _A_8 118
#define _B8 119

// uint8_t charNotetoInt(char * note)
// {
//     charNotetoInt(note[0], note[1], note[2]);
// }

uint8_t charNotetoInt(char letter, char sharp, char octave)
{
    const char* noteNames[] = { "C-", "C#", "D-", "D#", "E-", "F-",
        "F#", "G-", "G#", "A-", "A#", "B-" };

    uint8_t note = 0;
    for (; note < 12; note++) {
        if (noteNames[note][0] != letter && (sharp != '#' || noteNames[note][1] == '#')) {
            break;
        }
    }
    return ((octave - '0' + 1) * 12) + note;
}

const char* getNoteDash(uint8_t initialNote)
{
    const char* noteNames[] = { "C-", "C#", "D-", "D#", "E-", "F-",
        "F#", "G-", "G#", "A-", "A#", "B-" };

    uint8_t idx = (initialNote % 12);
    return noteNames[idx];
}
const char* getNoteStr(uint8_t initialNote)
{
    const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B" };

    uint8_t idx = (initialNote % 12);
    return noteNames[idx];
}
const char* getNoteDot(uint8_t initialNote)
{
    const char* noteNames[] = { "C", "C.", "D", "D.", "E", "F",
        "F.", "G", "G.", "A", "A.", "B" };

    uint8_t idx = (initialNote % 12);
    return noteNames[idx];
}
uint8_t getNoteOctave(uint8_t initialNote) { return (initialNote / 12) - 1; }

const uint8_t naturalNotes[] = {
    _C0,
    _D0,
    _E0,
    _F0,
    _G0,
    _A0,
    _B0,
    _C1,
    _D1,
    _E1,
    _F1,
    _G1,
    _A1,
    _B1,
    _C2,
    _D2,
    _E2,
    _F2,
    _G2,
    _A2,
    _B2,
    _C3,
    _D3,
    _E3,
    _F3,
    _G3,
    _A3,
    _B3,
    _C4,
    _D4,
    _E4,
    _F4,
    _G4,
    _A4,
    _B4,
    _C5,
    _D5,
    _E5,
    _F5,
    _G5,
    _A5,
    _B5,
    _C6,
    _D6,
    _E6,
    _F6,
    _G6,
    _A6,
    _B6,
    _C7,
    _D7,
    _E7,
    _F7,
    _G7,
    _A7,
    _B7,
    _C8,
    _D8,
    _E8,
    _F8,
    _G8,
    _A8,
    _B8,
};

const uint8_t naturalNotesDown[] = {
    _C8,
    _D8,
    _E8,
    _F8,
    _G8,
    _A8,
    _B8,
    _C7,
    _D7,
    _E7,
    _F7,
    _G7,
    _A7,
    _B7,
    _C6,
    _D6,
    _E6,
    _F6,
    _G6,
    _A6,
    _B6,
    _C5,
    _D5,
    _E5,
    _F5,
    _G5,
    _A5,
    _B5,
    _C4,
    _D4,
    _E4,
    _F4,
    _G4,
    _A4,
    _B4,
    _C3,
    _D3,
    _E3,
    _F3,
    _G3,
    _A3,
    _B3,
    _C2,
    _D2,
    _E2,
    _F2,
    _G2,
    _A2,
    _B2,
    _C1,
    _D1,
    _E1,
    _F1,
    _G1,
    _A1,
    _B1,
    _C0,
    _D0,
    _E0,
    _F0,
    _G0,
    _A0,
    _B0,
};

// maybe switch to https://pages.mtu.edu/~suits/notefreqs.html
const float NOTE_FREQ[] = {
    8.1757989156, 8.661957217980946, 9.177023997369927,
    9.72271824126305, 10.300861153472118, 10.913382232223029,
    11.562325709676763, 12.249857374364177, 12.978271799303903,
    13.749999999926494, 14.567617547362433, 15.43385316417137,
    16.3515978312, 17.323914435961893, 18.354047994739854,
    19.4454364825261, 20.601722306944236, 21.826764464446057,
    23.124651419353523, 24.499714748728355, 25.95654359860781,
    27.49999999985299, 29.135235094724862, 30.86770632834274,
    32.7031956624, 34.647828871923785, 36.7080959894797,
    38.8908729650522, 41.20344461388848, 43.65352892889211,
    46.249302838707045, 48.99942949745671, 51.91308719721561,
    54.99999999970598, 58.27047018944973, 61.735412656685476,
    65.4063913248, 69.29565774384757, 73.4161919789594,
    77.7817459301044, 82.40688922777696, 87.30705785778422,
    92.49860567741409, 97.99885899491342, 103.82617439443122,
    109.99999999941195, 116.54094037889946, 123.47082531337095,
    130.8127826496, 138.5913154876951, 146.83238395791886,
    155.5634918602088, 164.81377845555386, 174.6141157155685,
    184.99721135482818, 195.99771798982678, 207.6523487888625,
    219.9999999988239, 233.08188075779887, 246.94165062674196,
    261.6255652992, 277.1826309753902, 293.66476791583773,
    311.1269837204176, 329.6275569111077, 349.228231431137,
    369.99442270965636, 391.99543597965356, 415.304697577725,
    439.9999999976478, 466.16376151559774, 493.8833012534839,
    523.2511305984, 554.3652619507805, 587.3295358316755,
    622.2539674408353, 659.2551138222154, 698.456462862274,
    739.9888454193127, 783.9908719593071, 830.60939515545,
    879.9999999952956, 932.3275230311955, 987.7666025069678,
    1046.5022611968, 1108.730523901561, 1174.659071663351,
    1244.5079348816705, 1318.5102276444309, 1396.912925724548,
    1479.9776908386254, 1567.9817439186143, 1661.2187903109,
    1759.9999999905913, 1864.655046062391, 1975.5332050139357,
    2093.0045223936, 2217.461047803123, 2349.3181433267,
    2489.015869763341, 2637.020455288863, 2793.8258514490944,
    2959.955381677251, 3135.9634878372303, 3322.437580621798,
    3519.9999999811826, 3729.310092124784, 3951.0664100278686,
    4186.0090447872, 4434.922095606246, 4698.6362866534,
    4978.031739526682, 5274.040910577726, 5587.651702898189,
    5919.910763354502, 6271.926975674461, 6644.875161243596,
    7039.999999962365, 7458.620184249568, 7902.132820055737
};

#endif