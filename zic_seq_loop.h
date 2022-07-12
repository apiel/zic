#ifndef ZIC_SEQ_LOOP_H_
#define ZIC_SEQ_LOOP_H_

#include "zic_note.h"
#include "zic_seq_pattern.h"

class Zic_Seq_Loop_State {
public:
    int8_t detune = 0;
    uint8_t velocity = 100;
    Zic_Seq_Pattern* pattern = NULL;
    bool playing = false;

    void set(Zic_Seq_Loop_State* state)
    {
        detune = state->detune;
        velocity = state->velocity;
        pattern = state->pattern;
        playing = state->playing;
    }

    void set(int8_t _detune, uint8_t _velocity = 100)
    {
        velocity = _velocity;
        detune = range(_detune, -99, 99);
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
    uint8_t currentStep = 0;
    Zic_Seq_Step stepOn;
    Zic_Seq_Step stepOff;

public:
    Zic_Seq_Loop_State state;
    Zic_Seq_Loop_State nextState;

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

    bool wasSlide()
    {
        return stepOff.slide;
    }

    void next()
    {
        if (state.playing && state.pattern) {
            stepOff.set(&stepOn);
            stepOn.set(&state.pattern->steps[currentStep]);
            if (stepOn.note) {
                stepOn.note += state.detune;
            }
            stepOn.velocity = state.velocity;
            currentStep = (currentStep + 1) % state.pattern->stepCount;
        }

        if (currentStep == 0) {
            state.set(&nextState);
        }
    }
};

#endif
