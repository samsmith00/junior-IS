//
// Created by Sam Smith on 4/14/26.
//
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace pitchShifter {
    struct Parameters {
        explicit Parameters(juce::AudioProcessor&);

        juce::AudioParameterInt& pitchFactor;
    };
}