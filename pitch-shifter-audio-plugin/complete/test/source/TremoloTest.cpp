#include "TestUtils.h"
#include <gtest/gtest.h>
#include <tremolo_plugin/tremolo_plugin.h>

#include <wolfsound/file/wolfsound_WavFileWriter.hpp>
#include <wolfsound/dsp/wolfsound_testSignals.hpp>

namespace tremolo {
namespace {
void extractLfo(Tremolo& tremolo, juce::AudioBuffer<float>& bufferToUse) {
  juce::dsp::AudioBlock<float> block{bufferToUse};
  block.fill(1.f);
  tremolo.process(bufferToUse);
  block.subtract(1.f);
}
}  // namespace

/** This test extracts the LFO used by the Tremolo effect and saves it to a WAV
 * file "lfo.wav".
 *
 * You can find the file in the same folder where the test executable resides,
 * most probably [CMake binary dir]/test/.
 *
 * The LFO rate is not changed; thus, the one set in Tremolo's constructor will
 * be used.
 */
TEST(Tremolo, ExtractLfo) {
  for (const auto lfoWaveform :
       {Tremolo::LfoWaveform::sine, Tremolo::LfoWaveform::triangle}) {
    Tremolo testee;
    constexpr auto sampleRate = 48000.0;
    testee.setLfoWaveform(lfoWaveform);
    testee.prepare(sampleRate, static_cast<int>(sampleRate));

    juce::AudioBuffer<float> buffer;
    buffer.setSize(1, static_cast<int>(sampleRate));

    extractLfo(testee, buffer);

    const auto* const fileName = lfoWaveform == Tremolo::LfoWaveform::sine
                                     ? "sineLfo.wav"
                                     : "triangleLfo.wav";

    wolfsound::WavFileWriter::writeToFile(
        getFileOutputPath(fileName),
        juce::Span{buffer.getReadPointer(0),
                   static_cast<size_t>(buffer.getNumSamples())},
        wolfsound::Frequency{sampleRate});
  }
}

/** This test extracts the LFO used by the Tremolo effect switching the LFO
 * shape mid-processing, and saves it to a WAV file "smoothedLfo.wav".
 *
 * You can find the file in the same folder where the test executable resides,
 * most probably [CMake binary dir]/test/.
 *
 * The LFO rate is not changed; thus, the one set in Tremolo's constructor will
 * be used.
 */
TEST(Tremolo, LfoWaveformTransitionIsSmooth) {
  Tremolo testee;
  constexpr auto sampleRate = 48000.0;
  constexpr auto channelCount = 1;
  constexpr auto blockSizeSeconds = 1;
  constexpr auto blockSizeSamples =
      static_cast<int>(blockSizeSeconds * sampleRate);
  testee.prepare(sampleRate, blockSizeSamples);

  juce::AudioBuffer<float> outputBuffer;
  constexpr auto recordingLengthSamples = 2 * blockSizeSamples;
  outputBuffer.setSize(channelCount, recordingLengthSamples);

  juce::AudioBuffer<float> processBuffer;
  processBuffer.setSize(channelCount, blockSizeSamples);

  testee.setLfoWaveform(Tremolo::LfoWaveform::sine);
  extractLfo(testee, processBuffer);
  outputBuffer.copyFrom(0, 0, processBuffer, 0, 0, blockSizeSamples);
  testee.setLfoWaveform(Tremolo::LfoWaveform::triangle);
  extractLfo(testee, processBuffer);
  outputBuffer.copyFrom(0, blockSizeSamples, processBuffer, 0, 0,
                        blockSizeSamples);

  wolfsound::WavFileWriter::writeToFile(
      getFileOutputPath("smoothedLfo.wav"),
      juce::Span{outputBuffer.getReadPointer(0),
                 static_cast<size_t>(outputBuffer.getNumSamples())},
      wolfsound::Frequency{sampleRate});
}

TEST(Tremolo, SamplewiseAndChannelwiseProcessingYieldIdenticalResults) {
  using namespace wolfsound::literals;
  using namespace std::chrono_literals;

  const auto sampleRate = 48000_Hz;
  const auto testSignal = wolfsound::generateWhiteNoise(sampleRate, 1s, 0);

  // create 2 identical input signal buffers
  juce::AudioBuffer<float> samplewiseBuffer{
      1, static_cast<int>(testSignal.size())};
  std::ranges::copy(testSignal, samplewiseBuffer.getWritePointer(0));
  auto channelwiseBuffer = samplewiseBuffer;

  // create 2 identical Tremolo instances with default parameters
  Tremolo samplewiseTremolo;
  Tremolo channelwiseTremolo;

  samplewiseTremolo.prepare(sampleRate.value(),
                            static_cast<int>(testSignal.size()));
  channelwiseTremolo.prepare(sampleRate.value(),
                             static_cast<int>(testSignal.size()));

  // process the first using sample-wise processing
  samplewiseTremolo.process(samplewiseBuffer);

  // process the second using block-wise processing
  channelwiseTremolo.processChannelwise(channelwiseBuffer);

  for (const auto i : std::views::iota(0, samplewiseBuffer.getNumSamples())) {
    EXPECT_NEAR(samplewiseBuffer.getSample(0, i),
                channelwiseBuffer.getSample(0, i), 1e-5f);
  }
}
}  // namespace tremolo
