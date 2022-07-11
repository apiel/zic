#ifndef ZIC_SEQ_LOOP_H_
#define ZIC_SEQ_LOOP_H_

#include "zic_note.h"
#include "zic_seq_pattern.h"

#define REF_NOTE Zic::_NOTE_C4

#ifndef SEQ_LOOP_DEFAULT_VELOCITY
#define SEQ_LOOP_DEFAULT_VELOCITY 100
#endif

class Zic_Seq_Loop {
protected:
    uint8_t currentStep = 0;
    Zic_Seq_Step stepOn;
    Zic_Seq_Step stepOff;
    uint8_t velocity = SEQ_LOOP_DEFAULT_VELOCITY;
    uint8_t nextVelocity = SEQ_LOOP_DEFAULT_VELOCITY;

public:
    bool loopOn = false;
    uint8_t play = 0;
    uint8_t previousLoopNote = REF_NOTE;
    uint8_t nextToPlay = 0;

    Zic_Seq_Pattern* nextPattern = NULL;
    Zic_Seq_Pattern* pattern = NULL;

    Zic_Seq_Loop() { }
    Zic_Seq_Loop(Zic_Seq_Pattern* _pattern)
    {
        pattern = _pattern;
        nextPattern = _pattern;
    }

    void setNextPattern(Zic_Seq_Pattern* _nextPattern)
    {
        nextPattern = _nextPattern;
    }

    Zic_Seq_Step* getNoteOn()
    {
        if (play && stepOn.note > 0) {
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
        if (play && pattern) {
            stepOff.set(&stepOn);
            stepOn.set(&pattern->steps[currentStep]);
            if (stepOn.note) {
                stepOn.note += (int)play - (int)REF_NOTE;
            }
            stepOn.velocity = velocity;
            currentStep = (currentStep + 1) % pattern->stepCount;
        }

        if (currentStep == 0) {
            pattern = nextPattern;
            play = nextToPlay ? nextToPlay : 0;
            velocity = nextVelocity;
        }
    }

    void on(uint8_t note, uint8_t _velocity = SEQ_LOOP_DEFAULT_VELOCITY)
    {
        nextVelocity = _velocity;
        nextToPlay = note;
    }

    void off(uint8_t note)
    {
        if (!loopOn && note == nextToPlay) {
            nextToPlay = 0;
        }
    }

    void toggleLoopMode() { setLoopMode(!loopOn); }

    void setLoopMode(bool value = true)
    {
        loopOn = value;
        if (loopOn) {
            nextToPlay = previousLoopNote;
        } else {
            previousLoopNote = nextToPlay;
            nextToPlay = 0;
        }
    }
};

#endif
