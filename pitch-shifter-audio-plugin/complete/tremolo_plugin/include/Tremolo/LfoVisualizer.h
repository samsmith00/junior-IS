#pragma once

namespace tremolo {
class LfoVisualizer : public juce::Component {
public:
  using ReadAllLfoSamples = std::function<void(juce::AudioBuffer<float>&)>;
  using GetCurrentSampleRate = std::function<double()>;
  using IsBypassed = std::function<bool()>;

  LfoVisualizer(ReadAllLfoSamples readSamples,
                GetCurrentSampleRate getRate,
                IsBypassed getIsBypassed);

  void paint(juce::Graphics& g) override;

  void setCurveWidth(float w);
  void setCurveColor(juce::Colour c);

  void setBackgroundColor(juce::Colour c);

private:
  static constexpr auto pointsOnPath = 22050u;
  static constexpr auto periodsToPlotOf1HzWaveform = 4u;

  void update(double timestampSeconds);

  void updateLfoCurve(double timestampSeconds);

  void updateSamplesQueue(double timestampSeconds);

  [[nodiscard]] size_t getStride() const;

  void samplesToPath();

  /** @brief Creates a transform that maps current LFO curve to component bounds
   *
   * @detail The transform is based on following point mappings:
   *
   *   (0,ylim)                        -> (0,0) (left-top corner)
   *   (0,-ylim)                       -> (0, height) (left-bottom corner)
   *   (curve end X coordinate, -ylim) -> (component width, component height)
   *                                      (right-bottom corner)
   */
  [[nodiscard]] juce::AffineTransform getLfoCurveTransform() const;

  float curveWidth{4.f};
  juce::Colour curveColor{juce::Colours::black};
  juce::Colour backgroundColour{juce::Colours::white};
  ReadAllLfoSamples readAllLfoSamples;
  GetCurrentSampleRate getCurrentSampleRate;
  IsBypassed isBypassed;
  juce::AudioBuffer<float> buffer;
  juce::Path lfoCurve;

  detail::StridedQueue<float, pointsOnPath> lfoSamplesToPlot;

  std::optional<double> lastTimestampSeconds;
  juce::VBlankAttachment vblankAttachment{
      this, [this](double timestampSeconds) { update(timestampSeconds); }};
};
}  // namespace tremolo
