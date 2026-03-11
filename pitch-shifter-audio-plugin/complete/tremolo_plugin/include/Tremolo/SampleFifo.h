#pragma once

namespace tremolo {
/** A single-producer, single-consumer FIFO queue to retrieve a single channel
 * of samples from the audio thread */
template <typename SampleType>
class SampleFifo {
public:
  void prepare(double sampleRate) {
    // we want to provide enough capacity so that we don't miss a
    // sample at low fps.
    const auto sampleCapacity = static_cast<int>(1.0 * sampleRate);

    buffer.setSize(1, sampleCapacity);
    buffer.clear();
    fifo.setTotalSize(sampleCapacity);
  }

  void push(SampleType sample) {
    const auto scope = fifo.write(1);

    if (scope.blockSize1 > 0) {
      buffer.setSample(0, scope.startIndex1, sample);
    } else if (scope.blockSize2 > 0) {
      buffer.setSample(0, scope.startIndex2, sample);
    }
  }

  void popAll(juce::AudioBuffer<SampleType>& bufferToFill) {
    const auto sampleCount = fifo.getNumReady();

    // avoidReallocating = true, to avoid reallocations when the buffer size
    // does not increase
    bufferToFill.setSize(1, sampleCount, false, false, true);

    const auto scope = fifo.read(sampleCount);
    const auto* samplesToReadPtr = buffer.getReadPointer(0);
    auto* samplesToWritePtr = bufferToFill.getWritePointer(0);
    if (scope.blockSize1 > 0) {
      std::copy_n(samplesToReadPtr + scope.startIndex1, scope.blockSize1,
                  samplesToWritePtr);
    }

    if (scope.blockSize2 > 0) {
      std::copy_n(samplesToReadPtr + scope.startIndex2, scope.blockSize2,
                  samplesToWritePtr + scope.blockSize1);
    }
  }

  void reset() {
    fifo.reset();
    buffer.clear();
  }

private:
  static constexpr auto initialCapacity = 1024;
  juce::AbstractFifo fifo{initialCapacity};
  juce::AudioBuffer<SampleType> buffer{1, initialCapacity};
};
}  // namespace tremolo
