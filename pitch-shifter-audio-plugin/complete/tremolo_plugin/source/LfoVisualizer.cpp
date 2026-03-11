
namespace tremolo {

LfoVisualizer::LfoVisualizer(ReadAllLfoSamples readSamples,
                             GetCurrentSampleRate getRate,
                             IsBypassed getIsBypassed)
    : readAllLfoSamples{std::move(readSamples)},
      getCurrentSampleRate{std::move(getRate)},
      isBypassed{std::move(getIsBypassed)} {
  // preallocate
  buffer.setSize(1, static_cast<int>(getCurrentSampleRate()));

  samplesToPath();
}

void LfoVisualizer::paint(juce::Graphics& g) {
  g.fillAll(backgroundColour);

  g.setColour(curveColor);
  g.strokePath(lfoCurve,
               juce::PathStrokeType{curveWidth,
                                    juce::PathStrokeType::JointStyle::curved},
               getLfoCurveTransform());
}

void LfoVisualizer::setCurveWidth(float w) {
  curveWidth = w;
}

void LfoVisualizer::setCurveColor(juce::Colour c) {
  curveColor = c;
}

void LfoVisualizer::setBackgroundColor(juce::Colour c) {
  backgroundColour = c;
}

void LfoVisualizer::update(double timestampSeconds) {
  updateLfoCurve(timestampSeconds);
  repaint();
}

void LfoVisualizer::updateLfoCurve(double timestampSeconds) {
  updateSamplesQueue(timestampSeconds);
  samplesToPath();
}

void LfoVisualizer::updateSamplesQueue(double timestampSeconds) {
  if (!lastTimestampSeconds.has_value()) {
    lastTimestampSeconds = timestampSeconds;
    return;
  }

  readAllLfoSamples(buffer);

  lfoSamplesToPlot.setStride(getStride());

  const auto newAvailableSamples = buffer.getNumSamples();

  if (isBypassed()) {
    const auto secondsPassed = timestampSeconds - *lastTimestampSeconds;
    const auto samplesPassed =
        static_cast<size_t>(getCurrentSampleRate() * secondsPassed);
    lfoSamplesToPlot.pushBackZeros(samplesPassed);
  } else if (newAvailableSamples > 0) {
    lfoSamplesToPlot.pushBack(std::span{
        buffer.getReadPointer(0), static_cast<size_t>(buffer.getNumSamples())});
  }

  lastTimestampSeconds = timestampSeconds;
}

size_t LfoVisualizer::getStride() const {
  return static_cast<size_t>(getCurrentSampleRate() *
                             periodsToPlotOf1HzWaveform / pointsOnPath);
}

void LfoVisualizer::samplesToPath() {
  juce::Path path;
  path.startNewSubPath(0.f, lfoSamplesToPlot.front());
  for (const auto i : std::views::iota(1u, lfoSamplesToPlot.size())) {
    path.lineTo(static_cast<float>(i), lfoSamplesToPlot.at(i));
  }
  lfoCurve = path;
}

// clang-format off
juce::AffineTransform LfoVisualizer::getLfoCurveTransform() const {
  constexpr auto ylim = 1.1f;
  const auto bounds = getLocalBounds().toFloat();
  const auto transform = juce::AffineTransform::fromTargetPoints(
      0.f, ylim,                                   /* -> */ 0.f, 0.f,
      0.f, -ylim,                                  /* -> */ 0.f, bounds.getHeight(),
      lfoCurve.getCurrentPosition().getX(), -ylim, /* -> */ bounds.getWidth(), bounds.getHeight());
  return transform;
}
// clang-format on
}  // namespace tremolo
