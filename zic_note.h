#ifndef ZIC_NOTE_H_
#define ZIC_NOTE_H_

#include <stdint.h>

namespace Zic {
enum {
    _NOTE_START = 12,

    _NOTE_C0 = 12,
    _NOTE_Cs0 = 13,
    _NOTE_D0 = 14,
    _NOTE_Ds0 = 15,
    _NOTE_E0 = 16,
    _NOTE_F0 = 17,
    _NOTE_Fs0 = 18,
    _NOTE_G0 = 19,
    _NOTE_Gs0 = 20,
    _NOTE_A0 = 21,
    _NOTE_As0 = 22,
    _NOTE_B0 = 23,

    _NOTE_C1 = 24,
    _NOTE_Cs1 = 25,
    _NOTE_D1 = 26,
    _NOTE_Ds1 = 27,
    _NOTE_E1 = 28,
    _NOTE_F1 = 29,
    _NOTE_Fs1 = 30,
    _NOTE_G1 = 31,
    _NOTE_Gs1 = 32,
    _NOTE_A1 = 33,
    _NOTE_As1 = 34,
    _NOTE_B1 = 35,

    _NOTE_C2 = 36,
    _NOTE_Cs2 = 37,
    _NOTE_D2 = 38,
    _NOTE_Ds2 = 39,
    _NOTE_E2 = 40,
    _NOTE_F2 = 41,
    _NOTE_Fs2 = 42,
    _NOTE_G2 = 43,
    _NOTE_Gs2 = 44,
    _NOTE_A2 = 45,
    _NOTE_As2 = 46,
    _NOTE_B2 = 47,

    _NOTE_C3 = 48,
    _NOTE_Cs3 = 49,
    _NOTE_D3 = 50,
    _NOTE_Ds3 = 51,
    _NOTE_E3 = 52,
    _NOTE_F3 = 53,
    _NOTE_Fs3 = 54,
    _NOTE_G3 = 55,
    _NOTE_Gs3 = 56,
    _NOTE_A3 = 57,
    _NOTE_As3 = 58,
    _NOTE_B3 = 59,

    _NOTE_C4 = 60,
    _NOTE_Cs4 = 61,
    _NOTE_D4 = 62,
    _NOTE_Ds4 = 63,
    _NOTE_E4 = 64,
    _NOTE_F4 = 65,
    _NOTE_Fs4 = 66,
    _NOTE_G4 = 67,
    _NOTE_Gs4 = 68,
    _NOTE_A4 = 69,
    _NOTE_As4 = 70,
    _NOTE_B4 = 71,

    _NOTE_C5 = 72,
    _NOTE_Cs5 = 73,
    _NOTE_D5 = 74,
    _NOTE_Ds5 = 75,
    _NOTE_E5 = 76,
    _NOTE_F5 = 77,
    _NOTE_Fs5 = 78,
    _NOTE_G5 = 79,
    _NOTE_Gs5 = 80,
    _NOTE_A5 = 81,
    _NOTE_As5 = 82,
    _NOTE_B5 = 83,

    _NOTE_C6 = 84,
    _NOTE_Cs6 = 85,
    _NOTE_D6 = 86,
    _NOTE_Ds6 = 87,
    _NOTE_E6 = 88,
    _NOTE_F6 = 89,
    _NOTE_Fs6 = 90,
    _NOTE_G6 = 91,
    _NOTE_Gs6 = 92,
    _NOTE_A6 = 93,
    _NOTE_As6 = 94,
    _NOTE_B6 = 95,

    _NOTE_C7 = 96,
    _NOTE_Cs7 = 97,
    _NOTE_D7 = 98,
    _NOTE_Ds7 = 99,
    _NOTE_E7 = 100,
    _NOTE_F7 = 101,
    _NOTE_Fs7 = 102,
    _NOTE_G7 = 103,
    _NOTE_Gs7 = 104,
    _NOTE_A7 = 105,
    _NOTE_As7 = 106,
    _NOTE_B7 = 107,

    _NOTE_C8 = 108,
    _NOTE_Cs8 = 109,
    _NOTE_D8 = 110,
    _NOTE_Ds8 = 111,
    _NOTE_E8 = 112,
    _NOTE_F8 = 113,
    _NOTE_Fs8 = 114,
    _NOTE_G8 = 115,
    _NOTE_Gs8 = 116,
    _NOTE_A8 = 117,
    _NOTE_As8 = 118,
    _NOTE_B8 = 119,

    _NOTE_END = 119,
};

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
    _NOTE_C0,
    _NOTE_D0,
    _NOTE_E0,
    _NOTE_F0,
    _NOTE_G0,
    _NOTE_A0,
    _NOTE_B0,
    _NOTE_C1,
    _NOTE_D1,
    _NOTE_E1,
    _NOTE_F1,
    _NOTE_G1,
    _NOTE_A1,
    _NOTE_B1,
    _NOTE_C2,
    _NOTE_D2,
    _NOTE_E2,
    _NOTE_F2,
    _NOTE_G2,
    _NOTE_A2,
    _NOTE_B2,
    _NOTE_C3,
    _NOTE_D3,
    _NOTE_E3,
    _NOTE_F3,
    _NOTE_G3,
    _NOTE_A3,
    _NOTE_B3,
    _NOTE_C4,
    _NOTE_D4,
    _NOTE_E4,
    _NOTE_F4,
    _NOTE_G4,
    _NOTE_A4,
    _NOTE_B4,
    _NOTE_C5,
    _NOTE_D5,
    _NOTE_E5,
    _NOTE_F5,
    _NOTE_G5,
    _NOTE_A5,
    _NOTE_B5,
    _NOTE_C6,
    _NOTE_D6,
    _NOTE_E6,
    _NOTE_F6,
    _NOTE_G6,
    _NOTE_A6,
    _NOTE_B6,
    _NOTE_C7,
    _NOTE_D7,
    _NOTE_E7,
    _NOTE_F7,
    _NOTE_G7,
    _NOTE_A7,
    _NOTE_B7,
    _NOTE_C8,
    _NOTE_D8,
    _NOTE_E8,
    _NOTE_F8,
    _NOTE_G8,
    _NOTE_A8,
    _NOTE_B8,
};

const uint8_t naturalNotesDown[] = {
    _NOTE_C8,
    _NOTE_D8,
    _NOTE_E8,
    _NOTE_F8,
    _NOTE_G8,
    _NOTE_A8,
    _NOTE_B8,
    _NOTE_C7,
    _NOTE_D7,
    _NOTE_E7,
    _NOTE_F7,
    _NOTE_G7,
    _NOTE_A7,
    _NOTE_B7,
    _NOTE_C6,
    _NOTE_D6,
    _NOTE_E6,
    _NOTE_F6,
    _NOTE_G6,
    _NOTE_A6,
    _NOTE_B6,
    _NOTE_C5,
    _NOTE_D5,
    _NOTE_E5,
    _NOTE_F5,
    _NOTE_G5,
    _NOTE_A5,
    _NOTE_B5,
    _NOTE_C4,
    _NOTE_D4,
    _NOTE_E4,
    _NOTE_F4,
    _NOTE_G4,
    _NOTE_A4,
    _NOTE_B4,
    _NOTE_C3,
    _NOTE_D3,
    _NOTE_E3,
    _NOTE_F3,
    _NOTE_G3,
    _NOTE_A3,
    _NOTE_B3,
    _NOTE_C2,
    _NOTE_D2,
    _NOTE_E2,
    _NOTE_F2,
    _NOTE_G2,
    _NOTE_A2,
    _NOTE_B2,
    _NOTE_C1,
    _NOTE_D1,
    _NOTE_E1,
    _NOTE_F1,
    _NOTE_G1,
    _NOTE_A1,
    _NOTE_B1,
    _NOTE_C0,
    _NOTE_D0,
    _NOTE_E0,
    _NOTE_F0,
    _NOTE_G0,
    _NOTE_A0,
    _NOTE_B0,
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
}

#endif