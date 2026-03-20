//
// Created by Sam Smith on 3/18/26.
//

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <ranges>


namespace pitchShifter {
    class PitchShifter {
    public:
        PitchShifter() :
        fft(fftOrder),
        window(fftSize + 1, juce::dsp::WindowingFunction<float>::WindowingMethod:: hann, false){}

        int getLatencyInSamples() const;

        void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
            juce::ignoreUnused (sampleRate, expectedMaxFramesPerBlock);
            reset();
        }

        void process(float* channelData, int numSamples ) noexcept {

        }

        void reset() noexcept {
            writePtr = 0;
            hopCount = 0;
            std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0f);
            std::fill(outputBuffer.begin(), outputBuffer.end(), 0.0f);
        }

        private:
        // FFT variables
        static constexpr int fftOrder = 10;
        static constexpr int fftSize = 1 << fftOrder;      // 1024 samples
        static constexpr int numBins = fftSize / 2 + 1;    // 513 bins (Nyquist frequency)
        static constexpr int overlap = 4;                  // 75% overlap
        static constexpr int hopSize = fftSize / overlap;  // 256 samples

        // create fft and window objects
        juce::dsp::FFT fft;
        juce::dsp::WindowingFunction<float> window;

        // circular buffers
        int writePtr = 0;                        // keeps track of current write pos (POS IN BLOG)
        int hopCount = 0;                        // keeps track of when to perform fft (COUNT IN BLOG)
        std::array<float, fftSize> inputBuffer;  // contains input samples
        std::array<float, fftSize> outputBuffer; // contains output samples
        std::array<float, fftSize * 2> fftDat;    // pass data to and from FFT object







    };
}