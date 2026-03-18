//
// Created by Sam Smith on 3/18/26.
//

#pragma once
#include <juce_audio_basics/buffers/juce_AudioSampleBuffer.h>
#include <juce_core/maths/juce_MathsFunctions.h>

namespace pitchShifter {
    class PitchShifter {
    public:
        void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
            juce::ignoreUnused (sampleRate, expectedMaxFramesPerBlock);
        }

        void process(juce::AudioBuffer<float>& buffer) noexcept {
            for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples())) {

                for (const auto channelIndex:
                    std::views::iota(0, buffer.getNumChannels())) {
                    // get the input sample
                    const auto inputSample = buffer.getSample(channelIndex, frameIndex);

                    // pass to output sample
                    const auto outputSample = inputSample;

                    // set the output sample
                    buffer.setSample(channelIndex, frameIndex, outputSample);
                }
            }
        }

    };
}


#ifndef PITCHSHIFTER_PITCHSHIFTER_H
#define PITCHSHIFTER_PITCHSHIFTER_H

#endif //PITCHSHIFTER_PITCHSHIFTER_H