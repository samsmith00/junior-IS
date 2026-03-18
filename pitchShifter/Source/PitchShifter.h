//
// Created by Sam Smith on 3/18/26.
//

#pragma once
#include <juce_core/maths/juce_MathsFunctions.h>

namespace pitchShifter {
    class PitchShifter {
    public:
        void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
            juce::ignoreUnused (sampleRate, expectedMaxFramesPerBlock);
        }
    };
}


#ifndef PITCHSHIFTER_PITCHSHIFTER_H
#define PITCHSHIFTER_PITCHSHIFTER_H

#endif //PITCHSHIFTER_PITCHSHIFTER_H