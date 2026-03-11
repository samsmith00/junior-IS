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
      inputBuffer[inputBufferPtr] = buffer.getSample(0, frameIndex);
      //outputBuffer[inputBufferPtr] = 0.0f;
      inputBufferPtr++;

      // get output sample and clear the content back to 0
      float out = outputBuffer[outputBufferReadPtr];
      outputBuffer[outputBufferReadPtr] = 0;

      out *= windowCorrection;
      outputBufferReadPtr++;
      if (outputBufferReadPtr >= inputBuffer.size()) {
        outputBufferReadPtr = 0;
      }

      if (++hopCounter >= hopSize) {
        hopCounter = 0;
        // Process FFT

        outputBufferWritePtr = (outputBufferWritePtr + hopSize) % outputBuffer.size();
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
  unsigned int inputBufferPtr = 0;                             // keeps track of current write pos in input buffer.
  unsigned int hopCounter = 0;                                 // counts up to 256
  unsigned int outputBufferReadPtr = 0;                        // position for reading into buff
  unsigned int outputBufferWritePtr = 0;                       // position for writing from buff
  std::array<float, fftSize> inputBuffer;                      // Circular input buffer
  std::array<float, fftSize> outputBuffer;                     // Circular output buffer
  std::array<float, fftSize * 2> fftData;                      // array of complex values (a + bj), that is why we multiply the size by 2
  static constexpr float windowCorrection = hopSize / fftSize; // have to scale amplitude of output since there are several blocks overlapping


  void processFFT(std::array<float, fftSize> const&inputBuffer, unsigned int inputPos, std::array<float, fftSize> &outputBuffer, unsigned int outPos) noexcept {
    // making the fftData be in order from oldest to newest sample
    // first copy from inputPtr position to end of inputBuffer (oldest data)
    // second copy data up to inputPtr position and place at end of fftData (newest data)
    const float* inputPtr = inputBuffer.data();
    float* fftPtr = fftData.data();

    std::memcpy(fftPtr, inputPtr + inputBufferPtr, (fftSize - inputBufferPtr) * sizeof(float));
    if (pos > 0) {
      std::memcpy(fftPtr + fftSize - inputBufferPtr, inputPtr, inputBufferPtr * sizeof(float));
    }
  }

  void frameFFT() noexcept {
    const float* inputPtr = inputBuffer.data();
    float* fftPtr = fftData.data();

    std::memcpy(fftPtr, inputPtr + inputBufferPtr, (fftSize - inputBufferPtr) * sizeof(float)); // (void* dest, const void* src, count)
    if (inputBufferPtr > 0) {
      std::memcpy(fftPtr + fftSize - inputBufferPtr, inputPtr, inputBufferPtr * sizeof(float));
    }

    window.multiplyWithWindowingTable(fftPtr, fftSize);
    FFT.performRealOnlyForwardTransform(fftPtr, true); // just compute up to the Nyquist frequency
    // perform some sort of modification
    FFT.performRealOnlyInverseTransform(fftPtr);
    window.multiplyWithWindowingTable(fftPtr, fftSize);

    for (int i = 0; i < fftSize; ++i) {
      fftPtr[i] *= windowCorrection;
    }

    for (int i = 0; i < inputBufferPtr; ++i) {
      outputBuffer[i] += fftData[i + fftSize - inputBufferPtr];
    }

    for (int i = 0; i < fftSize - inputBufferPtr; ++i) {
      outputBuffer[i + inputBufferPtr] += fftData[i];
    }
  }

  // Reset FFT process
  void resetFFT() noexcept {
    inputBufferPtr = 0;
    hopCounter = 0;
    std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0f);
    std::fill(outputBuffer.begin(), outputBuffer.end(), 0.0f);
  }

  // You should put class members and private functions here
};
}  // namespace tremolo
