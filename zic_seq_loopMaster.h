#ifndef ZIC_SEQ_LOOP_MASTER_H_
#define ZIC_SEQ_LOOP_MASTER_H_

#include <stdint.h>

#include "zic_seq_lopp.h"

enum {
    SEQ_CONDITION_X0,
    SEQ_CONDITION_X1,
    SEQ_CONDITION_X2,
    SEQ_CONDITION_X3,
    SEQ_CONDITION_X4,
    SEQ_CONDITION_X5,
    SEQ_CONDITION_STOP,
    SEQ_CONDITION_MUTE,
}

class Zic_Seq_PatternComponent : public Zic_Seq_LoopState {
public:
    uint8_t condition = 0;
};

class Zic_Seq_LoopMaster : public Zic_Seq_Loop {
protected:
public:
};

#endif
