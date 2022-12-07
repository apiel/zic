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
    SEQ_CONDITION_RESTART,
    SEQ_CONDITIONS_COUNT,
};

const char* SEQ_CONDITIONS_NAMES[SEQ_CONDITIONS_COUNT] = {
    "x0",
    "x1",
    "x2",
    "x3",
    "x4",
    "x5",
    "! ",
    "<<",
};

const char* SEQ_CONDITIONS_FULLNAMES[SEQ_CONDITIONS_COUNT] = {
    "play in loop",
    "play once",
    "play twice",
    "play 3 times",
    "play 4 times",
    "play 5 times",
    "play & stop",
    "play & restart",
};

class Zic_Seq_PatternComponent : public Zic_Seq_LoopState {
public:
    uint8_t condition = 0;

    void set(Zic_Seq_PatternComponent* component)
    {
        Zic_Seq_LoopState::set(component);
        condition = component->condition;
    }

    void setCondition(int8_t _condition)
    {
        condition = (_condition + SEQ_CONDITIONS_COUNT) % SEQ_CONDITIONS_COUNT;
    }

    void setCondition(char* _condition)
    {
        for (uint8_t i = 0; i < SEQ_CONDITIONS_COUNT; i++) {
            // if (strncmp(_condition, SEQ_CONDITIONS_NAMES[i], 2) == 0) {
            if (_condition[0] == SEQ_CONDITIONS_NAMES[i][0] && _condition[1] == SEQ_CONDITIONS_NAMES[i][1]) {
                condition = i;
                return;
            }
        }
        condition = 0;
    }
};

class Zic_Seq_LoopMaster : public Zic_Seq_Loop {
protected:
    uint8_t componentCount;
    Zic_Seq_PatternComponent* components;
    uint8_t currentComponent = 0;
    int8_t loopInComponent = -1;
    bool playing = false;

    void setNextComponent()
    {
        loopInComponent = 0;
        currentComponent = (currentComponent + 1) % componentCount;
    }

    void backToFirstComponent()
    {
        loopInComponent = 0;
        currentComponent = 0;
    }

    void setLoopInComponent(uint8_t count)
    {
        if (loopInComponent < count - 1) {
            loopInComponent++;
        } else {
            setNextComponent();
        }
    }

public:
    Zic_Seq_LoopMaster(Zic_Seq_PatternComponent* _components, uint8_t count)
        : componentCount(count)
        , components(_components)
    {
        if (count > 0) {
            state.set(&components[0]);
        }
    }

    uint8_t getCurrentComponent()
    {
        return currentComponent;
    }

    bool isCurrentComponent(uint8_t index)
    {
        return currentComponent == index;
    }

    bool isComponentPlaying(uint8_t index)
    {
        return isCurrentComponent(index) && state.playing;
    }

    void setNextState() override
    {
        if (!playing) {
            state.stop();
            return;
        }

        Zic_Seq_PatternComponent* component = &components[currentComponent];
        switch (component->condition) {
        case SEQ_CONDITION_X0:
            break;
        case SEQ_CONDITION_X1:
            // setNextComponent(); // do not use else first step might be skipped on start
            setLoopInComponent(1);
            break;
        case SEQ_CONDITION_X2:
            setLoopInComponent(2);
            break;
        case SEQ_CONDITION_X3:
            setLoopInComponent(3);
            break;
        case SEQ_CONDITION_X4:
            setLoopInComponent(4);
            break;
        case SEQ_CONDITION_X5:
            setLoopInComponent(5);
            break;
        case SEQ_CONDITION_STOP:
            state.stop();
            return;
        case SEQ_CONDITION_RESTART:
            backToFirstComponent();
            break;
        }
        state.set(&components[currentComponent]);
        if (state.pattern != NULL && playing) {
            state.play();
        }
    }

    void togglePlay()
    {
        playing = !playing;
    }

    void reset()
    {
        playing = false;
        currentComponent = 0;
    }

    bool isPlaying()
    {
        return playing;
    }
};

#endif
