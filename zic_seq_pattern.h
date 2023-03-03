#ifndef ZIC_SEQ_PATTERN_H_
#define ZIC_SEQ_PATTERN_H_

#include "./zic_def.h"

#include <stdio.h>

#ifndef MAX_STEPS_IN_PATTERN
#define MAX_STEPS_IN_PATTERN 64
#endif

#ifndef MAX_VOICES_IN_PATTERN
#define MAX_VOICES_IN_PATTERN 4
#endif

#define PATTERN_NAME_SIZE 15
#define END 1

typedef uint8_t Pat[MAX_VOICES_IN_PATTERN][MAX_STEPS_IN_PATTERN][2];

#define STEP_CONDITION_MAX 107

class Zic_Seq_Step {
public:
    uint8_t instrument = 0;
    uint8_t note = 0;
    uint8_t velocity = 127;
    // TODO to be implemented in looper
    // -- , 01-99 , /2, /3, /4, /5, /6, /7, /8
    // 1 every step, 2 every 2nd step, 3 every 3rd step, 4 every 4th step... '!' could be only once
    // NEED probability
    // need also condition to play only the first time or the 2 first times '!' '!2'
    uint8_t condition = 0;
    uint16_t patchId = 0;
    bool tie = false;

    void reset()
    {
        instrument = 0;
        note = 0;
        velocity = 127;
        tie = false;
    }

    void set(Zic_Seq_Step* step)
    {
        set(step->instrument, step->note, step->velocity, step->tie, step->condition, step->patchId);
    }

    void set(uint8_t _note, bool _tie)
    {
        note = _note;
        tie = _tie;
    }

    void set(uint8_t _instrument, uint8_t _note, uint8_t _velocity, bool _tie, uint8_t _condition, uint16_t _patchId)
    {
        instrument = _instrument;
        note = _note;
        velocity = _velocity;
        tie = _tie;
        condition = _condition;
        patchId = _patchId;
    }

    void setCondition(int8_t _condition)
    {
        condition = range(_condition, 0, STEP_CONDITION_MAX);
    }

    void setCondition(char* _cond)
    {
        if (_cond[0] == '-') {
            condition = 0;
        } else if (_cond[0] == '/') {
            condition = _cond[1] - '2' + 100;
        } else {
            condition = atoi(_cond);
        }
    }

    // must be char[3]
    void getConditionName(char* name)
    {
        if (condition == 0) {
            name[0] = '-';
            name[1] = '-';
        } else if (condition < 100) {
            sprintf(name, "%02d", condition);
        } else {
            name[0] = '/';
            name[1] = '2' + condition - 100;
        }
        name[2] = '\0';
    }

    // must be char[22]
    void getConditionFullname(char* name)
    {
        if (condition == 0) {
            sprintf(name, "none");
        } else if (condition < 100) {
            sprintf(name, "mute probability %02d%%", condition);
        } else {
            sprintf(name, "play ever %d time", condition - 100 + 2);
        }
    }
};

class Zic_Seq_Pattern {
public:
    uint16_t id = 0;
    uint8_t stepCount = MAX_STEPS_IN_PATTERN;
    Zic_Seq_Step steps[MAX_VOICES_IN_PATTERN][MAX_STEPS_IN_PATTERN];

    Zic_Seq_Pattern()
    {
    }

    Zic_Seq_Pattern(Pat _steps)
    {
        for (uint8_t inst = 0; inst < MAX_VOICES_IN_PATTERN; inst++) {
            for (uint8_t pos = 0; pos < MAX_STEPS_IN_PATTERN; pos++) {
                if (_steps[inst][pos][0] == END) {
                    stepCount = pos;
                    break;
                }
                steps[inst][pos].set(_steps[inst][pos][0], _steps[inst][pos][1] == 1);
            }
        }
    }
};

#endif
