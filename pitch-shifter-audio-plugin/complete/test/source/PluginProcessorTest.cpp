#include "TestUtils.h"
#include <tremolo_plugin/tremolo_plugin.h>
#include <gtest/gtest.h>
#include <wolfsound/common/wolfsound_Frequency.hpp>
#include <wolfsound/file/wolfsound_WavFileWriter.hpp>

using namespace wolfsound::literals;

namespace tremolo {
/** This test doesn't test anything but shows that you can instantiate and use a
 * plugin processor in test */
TEST(PluginProcessor, PlaceholderTest) {
  PluginProcessor processor{};
}

class BypassTransitionIsSmoothTest : public testing::Test {
protected:
  void SetUp() override {
    testee.prepareToPlay(sampleRate.value(), blockSizeSamples);

    constexpr auto channelCount = 1;
    const auto recordingLengthSamples = 5 * blockSizeSamples;
    outputBuffer.setSize(channelCount, recordingLengthSamples);

    processBuffer.setSize(channelCount, blockSizeSamples);
  }

  wolfsound::Frequency sampleRate = 48_kHz;
  int blockSizeSamples{static_cast<int>(sampleRate.value())};
  PluginProcessor testee;

  juce::AudioBuffer<float> outputBuffer;
  juce::AudioBuffer<float> processBuffer;
};

TEST_F(BypassTransitionIsSmoothTest, ExerciseAllSegments) {
  auto process = [this, blockCount = 0]() mutable {
    juce::MidiBuffer midiBuffer{};
    juce::dsp::AudioBlock<float> block{processBuffer};
    block.fill(1.f);
    testee.processBlock(processBuffer, midiBuffer);
    block.subtract(1.f);

    jassert((blockCount + 1) * blockSizeSamples <=
            outputBuffer.getNumSamples());
    outputBuffer.copyFrom(0, blockCount * blockSizeSamples, processBuffer, 0, 0,
                          blockSizeSamples);

    blockCount++;
  };

  testee.getParameterRefs().bypassed = false;
  process();  // bypass OFF

  testee.getParameterRefs().bypassed = true;
  process();  // transition OFF -> ON
  process();  // bypass ON

  testee.getParameterRefs().bypassed = false;
  process();  // transition ON -> OFF
  process();  // bypass OFF

  wolfsound::WavFileWriter::writeToFile(
      getFileOutputPath("bypassTransition.wav"),
      juce::Span{outputBuffer.getReadPointer(0),
                 static_cast<size_t>(outputBuffer.getNumSamples())},
      sampleRate);
}
}  // namespace tremolo
