#ifndef ZIC_SEQ_LOOP_MASTER_H_
#define ZIC_SEQ_LOOP_MASTER_H_

#include <stdint.h>

#include "zic_seq_loop.h"

enum {
    SEQ_CONDITION_X0,
    SEQ_CONDITION_X1,
    SEQ_CONDITION_X2,
    SEQ_CONDITION_X3,
    SEQ_CONDITION_X4,
    SEQ_CONDITION_X5,
    SEQ_CONDITION_STOP,
    SEQ_CONDITION_MUTE,
    SEQ_CONDITIONS_COUNT,
};

const char *SEQ_CONDITIONS_NAMES[SEQ_CONDITIONS_COUNT] = {
    "x0",
    "x1",
    "x2",
    "x3",
    "x4",
    "x5",
    "! ",
    "--",
};

class Zic_Seq_PatternComponent : public Zic_Seq_LoopState {
public:
    uint8_t condition = 0;

    void setCondition(uint8_t _condition)
    {
        condition = _condition % SEQ_CONDITIONS_COUNT;
    }
};

class Zic_Seq_LoopMaster : public Zic_Seq_Loop {
protected:
    uint8_t componentCount;
    Zic_Seq_PatternComponent* components;
    uint8_t currentComponent = 0;

public:
    Zic_Seq_LoopMaster(Zic_Seq_PatternComponent* _components, uint8_t count)
        : componentCount(count)
        , components(_components)
    {
        if (count > 0) {
            state.set(&components[0]);

            // to be removed
            nextState.set(&components[0]);
        }
    }

    void setNextState() override
    {
        if (state.playing || nextState.playing) {
            if (nextState.currentStepSync == NULL) {
                state.set(&nextState);
            } else if (*nextState.currentStepSync == 0) {
                nextState.currentStepSync = NULL;
                state.set(&nextState);
            }
        }
    }
};

#endif
