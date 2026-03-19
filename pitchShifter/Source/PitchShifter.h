//
// Created by Sam Smith on 3/18/26.
//

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <ranges>


namespace pitchShifter {
    class PitchShifter {
    public:
        PitchShifter();
        int getLatencyInSamples() const;

        void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
            juce::ignoreUnused (sampleRate, expectedMaxFramesPerBlock);

          reset();
        }

        void process(float* channelData, int numSamples ) noexcept {

        }

        void reset() noexcept {}

        private:


    };
}