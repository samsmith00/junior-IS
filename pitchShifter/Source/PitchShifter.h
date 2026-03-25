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

        void process(float* channelData, int numSamples) noexcept {
            // loop over channel samples
            for (int i = 0; i < numSamples; ++i) {
                channelData[i] = processSample(channelData[i]);
            }
        }

        void reset() noexcept {
            inputWritePtr = 0;
            inputHopCount = 0;
            outputWritePtr = hopSize;
            outputReadPtr = 0;
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

        static constexpr float windowCorrection = 2.0f / 3.0f;


        // circular buffers
        int writePtr = 0;                         // keeps track of current write pos (POS IN BLOG)
        int hopCount = 0;                         // keeps track of when to perform fft (COUNT IN BLOG)
        std::array<float, fftSize> inputBuffer;   // contains input samples
        std::array<float, fftSize> outputBuffer;  // contains output samples
        std::array<float, fftSize * 2> fftData;   // pass data to and from FFT object


        // processes channel's samples
        // write input sample to input buffer
        // computes fft if hopCount reaches hopSize
        // returns output sample to write back to main Audio Buffer
        float processSample(float sample) {
            inputBuffer[writePtr] = sample;

            float outputSample = outputBuffer[writePtr];
            outputBuffer[writePtr] = 0.0f;

            writePtr++;
            if (writePtr >= fftSize) {
                writePtr = 0;
            }

            hopCount++;
            if (hopCount >= hopSize) {
                hopCount = 0;
                processFrame();
            }

            return outputSample;
        }

        // compute fft of signal chunk
        void processFrame() {
            const float* inputPtr = inputBuffer.data();
            float* fftPtr = fftData.data();

            // unwrap inputBuffer into fftData (chronological order)
            for (int i = 0; i < fftSize; ++i) {
                int bufferIndex = (writePtr + i) % fftSize;
                fftData[i] = inputPtr[bufferIndex];
            }

            window.multiplyWithWindowingTable(fftPtr, fftSize);

            fft.performRealOnlyForwardTransform(fftPtr, true);

            fft.performRealOnlyInverseTransform(fftPtr);

            window.multiplyWithWindowingTable(fftPtr, fftSize);

            for (int i = 0; i < fftSize; ++i) {
                fftPtr[i] *= windowCorrection;
            }

            for (int i = 0; i < writePtr; ++i) {
                outputBuffer[i] = fftData[i + fftSize - writePtr];
            }

            for (int i = 0; i < writePtr; ++i) {
                outputBuffer[i + writePtr] = fftData[i];
            }


        }




    };
}