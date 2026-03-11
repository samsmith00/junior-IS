#pragma once

namespace tremolo {
enum class ApplySmoothing { no, yes };

class Tremolo {
public:
  enum class LfoWaveform : size_t {
    sine = 0,
    triangle = 1,
  };

  Tremolo() { setModulationRateHz(5.f, ApplySmoothing::no); }

  void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
    const juce::dsp::ProcessSpec processSpec{
        .sampleRate = sampleRate,
        .maximumBlockSize =
            static_cast<juce::uint32>(expectedMaxFramesPerBlock),
        .numChannels = 1u,
    };
    for (auto& lfo : lfos) {
      lfo.prepare(processSpec);
    }
    lfoSampleFifo.prepare(sampleRate);
    lfoTransitionSmoother.reset(sampleRate, 0.025 /* 25 milliseconds */);

    // allocate defensively
    lfoSamples.resize(4u * static_cast<size_t>(expectedMaxFramesPerBlock));
  }

  void setModulationRateHz(
      float rateHz,
      ApplySmoothing applySmoothing = ApplySmoothing::yes) noexcept {
    const auto force = applySmoothing == ApplySmoothing::no;
    for (auto& lfo : lfos) {
      lfo.setFrequency(rateHz, force);
    }
  }

  void setLfoWaveform(LfoWaveform waveform,
                      ApplySmoothing applySmoothing = ApplySmoothing::yes) {
    jassert(waveform == LfoWaveform::sine || waveform == LfoWaveform::triangle);

    lfoToSet = waveform;

    if (applySmoothing == ApplySmoothing::no) {
      currentLfo = waveform;
    }
  }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    // actual updating of the LFO waveform happens in process()
    // to keep setLfoWaveform() idempotent
    updateLfoWaveform();

    // for each frame
    for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples())) {
      // generate the LFO value
      const auto lfoValue = getNextLfoValue();
      lfoSampleFifo.push(lfoValue);

      // calculate the modulation value
      const auto modulationValue = modulationDepth * lfoValue + 1.f;

      for (const auto channelIndex :
           std::views::iota(0, buffer.getNumChannels())) {
        // get the input sample
        const auto inputSample = buffer.getSample(channelIndex, frameIndex);

        // modulate the sample
        const auto outputSample = modulationValue * inputSample;

        // set the output sample
        buffer.setSample(channelIndex, frameIndex, outputSample);
      }
    }
  }

  void processChannelwise(juce::AudioBuffer<float>& buffer) noexcept {
    // actual updating of the LFO waveform happens in process()
    // to keep setLfoWaveform() idempotent
    updateLfoWaveform();

    const auto samplesToProcess = std::min(
        lfoSamples.size(), static_cast<size_t>(buffer.getNumSamples()));

    // detect if the host is misbehaving; if this fails, then many more frames
    // have been given for processing than declared in prepare()
    jassert(samplesToProcess <= lfoSamples.size());

    // generate LFO signal
    for (const auto i : std::views::iota(0u, samplesToProcess)) {
      lfoSamples[i] = getNextLfoValue();
      lfoSampleFifo.push(lfoSamples[i]);
    }

    // calculate the modulation value
    juce::FloatVectorOperations::multiply(lfoSamples.data(), modulationDepth,
                                          samplesToProcess);
    juce::FloatVectorOperations::add(lfoSamples.data(), 1.f, samplesToProcess);

    // for each channel
    for (const auto channelIndex :
         std::views::iota(0, buffer.getNumChannels())) {
      juce::FloatVectorOperations::multiply(
          buffer.getWritePointer(channelIndex), lfoSamples.data(),
          samplesToProcess);
    }
  }

  void reset() noexcept {
    for (auto& lfo : lfos) {
      lfo.reset();
    }
    lfoSampleFifo.reset();
  }

  void readAllLfoSamples(juce::AudioBuffer<float>& bufferToFill) {
    lfoSampleFifo.popAll(bufferToFill);
  }

private:
  static constexpr auto modulationDepth = 0.4f;

  static float triangle(float phase) {
    // offset the phase by pi/2 to return 0 if phase equals 0
    // and match the sine waveform
    // (otherwise, the waveform starts at 1)
    const auto offsetPhase = phase - juce::MathConstants<float>::halfPi;

    // Source:
    // https://thewolfsound.com/sine-saw-square-triangle-pulse-basic-waveforms-in-synthesis/#triangle
    const auto ft = offsetPhase / juce::MathConstants<float>::twoPi;
    return 4.f * std::abs(ft - std::floor(ft + 0.5f)) - 1.f;
  }

  void updateLfoWaveform() {
    if (lfoToSet != currentLfo) {
      // update the smoother
      lfoTransitionSmoother.setCurrentAndTargetValue(getNextLfoValue());

      currentLfo = lfoToSet;

      // initiate smoothing
      lfoTransitionSmoother.setTargetValue(getNextLfoValue());
    }
  }

  float getNextLfoValue() {
    if (lfoTransitionSmoother.isSmoothing()) {
      return lfoTransitionSmoother.getNextValue();
    }
    // the argument is added to the generated sample, thus, we pass in 0
    // to get just the generated sample
    return lfos[juce::toUnderlyingType(currentLfo)].processSample(0.f);
  }

  std::array<juce::dsp::Oscillator<float>, 2u> lfos{
      juce::dsp::Oscillator<float>{[](auto phase) {
        // start phase is -pi -> change it to 0 to match the mathematical sine
        return std::sin(phase + juce::MathConstants<float>::pi);
      }},
      juce::dsp::Oscillator<float>{triangle}};

  LfoWaveform currentLfo = LfoWaveform::sine;
  LfoWaveform lfoToSet = currentLfo;

  juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>
      lfoTransitionSmoother{0.f};
  std::vector<float> lfoSamples;

  SampleFifo<float> lfoSampleFifo;
};
}  // namespace tremolo
