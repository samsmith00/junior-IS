#pragma once
#include <unistd.h>

namespace tremolo {
class Tremolo {
public:
  // class constructor
  Tremolo() :
    FFT(fftOrder),
    window(fftSize + 1, juce::dsp::WindowingFunction<float>::WindowingMethod::hann, false)
  {
    resetFFT();
  }

  void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
    //juce::ignoreUnused(sampleRate, expectedMaxFramesPerBlock);
    //inputBuffer.resize(static_cast<unsigned int>(expectedMaxFramesPerBlock));
  }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    // for each frame
    for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples())) {

      // Populate circular buffer with samples for FFT computation
      inputBuffer[readPointer] = buffer.getSample(0, frameIndex);
      outputBuffer[readPointer] = 0.0f;
      readPointer++;

      if (readPointer == fftSize) {
        readPointer = 0;
      }

      hopPos++;
      if (hopPos == hopSize) {
        hopPos = 0;
        // PERFORM FFT
      }



      // TODO: generate the LFO value
      // TODO: calculate the modulation value

      /*
       * Dont need channels for guitar plugin

      //for each channel sample in the frame
      for (const auto channelIndex :
           std::views::iota(0, buffer.getNumChannels())) {
        // get the input sample
        const auto inputSample = buffer.getSample(channelIndex, frameIndex);

        // TODO: modulate the sample
        const auto outputSample = inputSample;

        // set the output sample
        buffer.setSample(channelIndex, frameIndex, outputSample);
      }
      */
    }
  }

  void reset() noexcept {}

private:
  // FFT Parameters
  static constexpr auto fftOrder = 10;
  static constexpr int fftSize = 1 << fftOrder;      // 1024 samples
  static constexpr int numBins = (fftSize / 2) + 1;  // 513 bins
  static constexpr int overlap = 4;                  // 75% overlap
  static constexpr int hopSize = fftSize / overlap;  // 256 samples

  juce::dsp::FFT FFT;
  juce::dsp::WindowingFunction<float> window;

  // Circular Buffer Initialization
  unsigned int readPointer = 0;            // keeps track of current write pos in input buffer.
  unsigned int hopPos = 0;                 // counts up to 256
  std::array<float, fftSize> inputBuffer;  // Circular input buffer
  std::array<float, fftSize> outputBuffer; // Circular output buffer
  std::array<float, fftSize * 2> fftData;  // array of complex values (a + bj), that is why we multiply the size by 2
  static constexpr float windowCorrection = 2.0f / 3.0f;


  void frameFFT() noexcept {
    const float* inputPtr = inputBuffer.data();
    float* fftPtr = fftData.data();

    std::memcpy(fftPtr, inputPtr + readPointer, (fftSize - readPointer) * sizeof(float)); // (void* dest, const void* src, count)
    if (readPointer > 0) {
      std::memcpy(fftPtr + fftSize - readPointer, inputPtr, readPointer * sizeof(float));
    }

    window.multiplyWithWindowingTable(fftPtr, fftSize);
    FFT.performRealOnlyForwardTransform(fftPtr, true); // just compute up to the Nyquist frequency
    // perform some sort of modification
    FFT.performRealOnlyInverseTransform(fftPtr);
    window.multiplyWithWindowingTable(fftPtr, fftSize);

    for (int i = 0; i < fftSize; ++i) {
      fftPtr[i] *= windowCorrection;
    }

    for (int i = 0; i < readPointer; ++i) {
      outputBuffer[i] += fftData[i + fftSize - readPointer];
    }

    for (int i = 0; i < fftSize - readPointer; ++i) {
      outputBuffer[i + readPointer] += fftData[i];
    }
  }

  // Reset FFT process
  void resetFFT() noexcept {
    readPointer = 0;
    hopPos = 0;
    std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0f);
    std::fill(outputBuffer.begin(), outputBuffer.end(), 0.0f);
  }

  // You should put class members and private functions here
};
}  // namespace tremolo
