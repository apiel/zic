#ifndef ZIC_SEQ_LOOP_H_
#define ZIC_SEQ_LOOP_H_

#include "zic_note.h"
#include "zic_seq_pattern.h"

class Zic_Seq_LoopState {
public:
    int8_t detune = 0;
    Zic_Seq_Pattern* pattern = NULL;
    bool playing = false;
    uint8_t* currentStepSync = NULL;

    void set(Zic_Seq_LoopState* state)
    {
        detune = state->detune;
        pattern = state->pattern;
        playing = state->playing;
        currentStepSync = state->currentStepSync;
    }

    void set(int8_t _detune)
    {
        setDetune(_detune);
    }

    void setDetune(int8_t _detune)
    {
        detune = range(_detune, -35, 35);
    }

    void setPattern(Zic_Seq_Pattern* _pattern)
    {
        pattern = _pattern;
    }

    void togglePlay() { play(!playing); }

    void stop()
    {
        play(false);
    }

    void play(bool value = true)
    {
        playing = value;
    }
};

class Zic_Seq_Loop {
protected:
    Zic_Seq_Step stepOn;
    Zic_Seq_Step stepOff;

    virtual void setNextState()
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

    void setNextStep()
    {
        currentStep = (currentStep + 1) % state.pattern->stepCount;
    }

public:
    uint8_t currentStep = 0;
    Zic_Seq_LoopState state;
    Zic_Seq_LoopState nextState;

    Zic_Seq_Loop() { }
    Zic_Seq_Loop(Zic_Seq_Pattern* _pattern)
    {
        state.setPattern(_pattern);
        nextState.setPattern(_pattern);
    }

    Zic_Seq_Step* getNoteOn()
    {
        if (state.playing && stepOn.note > 0) {
            return &stepOn;
        }
        return NULL;
    }

    Zic_Seq_Step* getNoteOff()
    {
        if (stepOff.slide && getNoteOn()) {
            return NULL;
        }

        // TODO bad idea, instead use something like `step.isOn` or `step.playing`
        // // To avoid to trigger off again set slide to true
        // stepOff.slide = true;
        return &stepOff;
    }

    uint8_t getStepsLeft()
    {
        return state.pattern->stepCount - (currentStep + 1);
    }

    bool wasSlide()
    {
        return stepOff.slide;
    }

    virtual void next()
    {
        stepOff.set(&stepOn);
        if (state.pattern && state.playing) {
            stepOn.set(&state.pattern->steps[currentStep]);
            if (stepOn.note) {
                stepOn.note += state.detune;
            }
            setNextStep();
        }

        if (currentStep == 0) {
            setNextState();
        }
    }
};

#endif
