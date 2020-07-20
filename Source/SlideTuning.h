#ifndef SLIDETUNING_H
#define SLIDETUNING_H

#include <JuceHeader.h>
#include "BirlOSCReceiver.h"
#include "Birl_Tuning.h"

// Takes the fingering from the Birl OSC Receiver and retunes according
// to an equal-tempered Rule-Based algorithm.

// Inputs the keys_[] data and outputs a fractional interval less than an octave.
static double getMidiAdjustment() {
    double midiAdjustment = 12.0;
    if (keys_[0] > 0.0) {
        midiAdjustment -= keys_[0];
        if (keys_[0] >= CLIP_RATIO) {
            midiAdjustment -= 2 * keys_[1];
            if (keys_[1] >= CLIP_RATIO) {
                midiAdjustment -= 2 * keys_[2];
            }
        }
        if (keys_[1] >= CLIP_RATIO && keys_[2] >= CLIP_RATIO) {
            if (keys_[4] > 0.0) {
                midiAdjustment -= 2 * keys_[4];
                if (keys_[4] >= CLIP_RATIO) {
                    midiAdjustment -= keys_[5];
                    if (keys_[5] >= CLIP_RATIO) {
                        midiAdjustment -= 2 * keys_[6];
                        if (keys_[6] >= CLIP_RATIO) {
                            midiAdjustment -= keys_[7];
                            midiAdjustment -= 2 * keys_[8];
                        }
                    }
                }
            }
            if (keys_[4] == 0.0 && keys_[5] > 0.0) {
                midiAdjustment -= keys_[5];
            }
        }
    }
    else if (keys_[1] > 0.0) {
        midiAdjustment -= keys_[1];
    }
    midiAdjustment += keys_[3];
    return midiAdjustment;
}

double getOctaveAdjustment() {
    if (buttons_[0]) {
        return 12.0;
    } else if (buttons_[1]){
        return -12.0;
    } else {
        return 0.0;
    }
}

    
double getEqualTemperedRatio () {
    double semitone = pow(2.0, 1.0/12.0);
    double wholetone = semitone * semitone;
//    return wholetone - semitone;
    return wholetone*2;
}

double getToneholeFrequency(double midiAdjustment, double fc, double octave_midi) {
    double fc_midi = LEAF_frequencyToMidi(fc);
    double note_midi = fc_midi + midiAdjustment + octave_midi;
    
    
    // split midiAdjustment into its floor and decimal part
    double note_whole;
    double note_decimals = modf(note_midi, &note_whole);

    return LEAF_midiToFrequency(note_whole + note_decimals
                                               * getEqualTemperedRatio());
}
    


#endif
