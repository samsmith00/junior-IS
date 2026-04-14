//
// Created by Sam Smith on 3/18/26.
//

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <ranges>
#include <iostream>
#include <launch.h>


namespace pitchShifter {
    class PitchShifter {
    public:
        PitchShifter() :
        fft(fftOrder),
        window(fftSize, juce::dsp::WindowingFunction<float>::hann)
        {
        }

        static int getLatencyInSamples() {
            return fftSize;
        }

        void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
            juce::ignoreUnused (expectedMaxFramesPerBlock);
            psSampleRate = sampleRate;
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
            std::fill(previousPhaseData.begin(), previousPhaseData.end(), 0.0f);
            std::fill(previousSynthesizedPhaseData.begin(), previousSynthesizedPhaseData.end(), 0.0f);
            std::fill(analysisMagnitudes.begin(), analysisMagnitudes.end(), 0.0f);
            std::fill(analysisFrequencies.begin(), analysisFrequencies.end(), 0.0f);
            std::fill(synthesisMagnitudes.begin(), synthesisMagnitudes.end(), 0.0f);
            std::fill(synthesisFrequencies.begin(), synthesisFrequencies.end(), 0.0f);
        }

        private:
        // FFT variables
        static constexpr int fftOrder = 10;
        static constexpr int fftSize = 1 << fftOrder;      // 1024 samples
        static constexpr int overlap = 4;                  // 75% overlap
        static constexpr int hopSize = fftSize / overlap;  // 256 samples
        double psSampleRate;

        // create fft and window objects
        juce::dsp::FFT fft;
        juce::dsp::WindowingFunction<float> window;

        static constexpr float windowCorrection = static_cast<float>(hopSize) / static_cast<float>(fftSize);


        // circular buffers
        int inputWritePtr = 0;                     // keeps track of current write pos (POS IN BLOG)
        int inputHopCount = 0;                     // keeps track of when to perform fft (COUNT IN BLOG)

        int outputWritePtr = hopSize;
        int outputReadPtr = 0;
        static constexpr int bufferSize = 4096;

        std::array<float, bufferSize> inputBuffer;   // contains input samples
        std::array<float, bufferSize> outputBuffer;  // contains output samples, size larger than needs to be
        std::array<float, fftSize * 2> fftData;      // pass data to and from FFT object


        // phase calculation variables
        //int nyquistFrequency = fftSize / 2;

        std::array<float, fftSize> previousPhaseData;
        std::array<float, fftSize> previousSynthesizedPhaseData;
        std::array<float, fftSize/2+1> analysisMagnitudes;
        std::array<float, fftSize/2+1> analysisFrequencies;
        std::array<float, fftSize/2+1> synthesisMagnitudes;
        std::array<float, fftSize/2+1> synthesisFrequencies;

        // processes channel's samples
        // write input sample to input buffer
        // computes fft if hopCount reaches hopSize
        // returns output sample to write back to main Audio Buffer
        float processSample(float sample) {
            inputBuffer[inputWritePtr] = sample;

            inputWritePtr++;
            if (inputWritePtr >= bufferSize) {
                inputWritePtr = 0;
            }

            float outputSample = outputBuffer[outputReadPtr];
            outputBuffer[outputReadPtr] = 0;

            outputReadPtr++;
            if (outputReadPtr >= bufferSize) { // or fftSize
                outputReadPtr = 0;
            }

            inputHopCount++;
            if (inputHopCount >= hopSize) {
                inputHopCount = 0;
                processFrame();
                outputWritePtr = (outputWritePtr + hopSize) % bufferSize; // or fftSize
            }

            // write to output audio buffer or return what needs to be written
            return outputSample;
        }

        // compute fft of signal chunk
        void processFrame() {
            const float* inputPtr = inputBuffer.data();
            float* fftPtr = fftData.data();

            // unwrap inputBuffer into fftData (look backwards one window size)
            for (int i = 0; i < fftSize; ++i) {
                int bufferIndex = (inputWritePtr + i - fftSize + bufferSize) % bufferSize;
                fftData[i] = inputPtr[bufferIndex];
            }

            window.multiplyWithWindowingTable(fftPtr, fftSize);

            fft.performRealOnlyForwardTransform(fftPtr, true);

            pitchShift();

            fft.performRealOnlyInverseTransform(fftPtr);

            window.multiplyWithWindowingTable(fftPtr, fftSize);

            for (int i = 0; i < fftSize; ++i) {
                fftPtr[i] *= windowCorrection;
            }

            // copy new block of samples into output buffer
            for (int i = 0; i < fftSize; ++i) {
                int idx = (i + outputWritePtr) % bufferSize;
                outputBuffer[idx] += fftData[i];
            }
        }

        void pitchShift() {
            float* fftPtr = fftData.data();

            // Frequency Estimation
            for (int k = 0; k < fftSize/2; ++k) {
                int i = k * 2;

                float magnitude = std::sqrt(std::powf(fftPtr[i], 2) + std::powf(fftPtr[i + 1], 2));
                float phase = atan2(fftPtr[i + 1], fftPtr[i]);

                // calculate phase remainder for a given bin i
                float phaseDiff = phase - previousPhaseData[k];

                float expectedCenterFreq = (2 * M_PI * k * hopSize) / fftSize;
                float phaseRemainder = wraphPhase(phaseDiff - expectedCenterFreq);

                // calculate fractional bin
                float fractionalBin = ((phaseRemainder * fftSize) / (2 * M_PI * hopSize)) + (float)k;

                analysisMagnitudes[k] = magnitude;
                analysisFrequencies[k] = fractionalBin;

                previousPhaseData[k] = phase;
            }

            // pitch shifting
            std::ranges::fill(synthesisMagnitudes, 0);
            std::ranges::fill(synthesisFrequencies, 0);

            float pitchFactor = 0.5; // hard code pitch shifter

            for (int k = 0; k < fftSize/2; ++k) {
                // nearest bin to shifted frequency
                int newBin = floorf(k * pitchFactor + 0.5);

                float shiftedFrequency = analysisFrequencies[k] * pitchFactor;

                if (newBin <= fftSize/2) {
                    synthesisMagnitudes[newBin] += analysisMagnitudes[k];
                    synthesisFrequencies[newBin] = shiftedFrequency;
                }
            }

            // synthesis (phase -> frequency)
            for (int k = 0; k < fftSize/2; ++k) {
                float magnitude = synthesisMagnitudes[k];

                float binDeviation = synthesisFrequencies[k] - k;

                float phaseDiff = (2 * M_PI * hopSize * binDeviation) / fftSize;

                float idealPhase = (2 * M_PI * k * hopSize) / fftSize;

                float actualPhase = phaseDiff + idealPhase;

                float newPhase = wraphPhase(previousSynthesizedPhaseData[k] + actualPhase);

                float real = magnitude * cosf(newPhase);
                float imag = magnitude * sinf(newPhase);

                int i = k * 2;
                fftPtr[i] = real;
                fftPtr[i + 1] = imag;

                if (k > 0 && k < fftSize/2) {
                    int conjugate = (k + fftSize/2) * 2;
                    fftPtr[conjugate] = fftPtr[k];
                    fftPtr[conjugate + 1] = -fftPtr[k+1];
                }

                previousSynthesizedPhaseData[k] = newPhase;
            }
        }

        float wraphPhase(float phase) {
            if (phase >= 0) {
                return fmodf(phase + M_PI, 2 * M_PI) - M_PI;
            }
            else {
                return fmodf(phase - M_PI, 2 * M_PI) + M_PI;
            }

        }

    };
}
