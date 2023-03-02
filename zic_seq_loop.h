#ifndef ZIC_SEQ_LOOP_H_
#define ZIC_SEQ_LOOP_H_

#include "zic_note.h"
#include "zic_seq_patch.h"
#include "zic_seq_pattern.h"

class Zic_Seq_LoopState {
public:
    int8_t detune = 0;
    Zic_Seq_Pattern* pattern = NULL;
    Zic_Seq_Patch patch;
    bool playing = false;
    uint8_t* currentStepSync = NULL;
    uint16_t dataId = 0;

    void set(Zic_Seq_LoopState* state)
    {
        detune = state->detune;
        pattern = state->pattern;
        playing = state->playing;
        currentStepSync = state->currentStepSync;
        dataId = state->dataId;
        patch.set(state->patch);
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
    uint8_t stepOn = 255;
    uint8_t stepOff = 255;

    Zic_Seq_Loop() { }
    Zic_Seq_Loop(Zic_Seq_Pattern* _pattern)
    {
        state.setPattern(_pattern);
        nextState.setPattern(_pattern);
    }

    uint8_t getStepsLeft()
    {
        return state.pattern->stepCount - (currentStep + 1);
    }

    virtual bool next()
    {
        stepOff = stepOn;
        if (state.pattern && state.playing) {
            stepOn = currentStep;
            setNextStep();
        }

        if (currentStep == 0) {
            setNextState();
            return true;
        }
        return false;
    }
};

#endif
