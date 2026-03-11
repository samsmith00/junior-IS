namespace tremolo {
void JsonSerializer::serialize(const Parameters& parameters,
                               juce::OutputStream& output) {
  juce::ignoreUnused(parameters, output);

  // TODO: serialize parameters to the output stream as JSON
}

juce::Result JsonSerializer::deserialize(juce::InputStream& input,
                                         Parameters& parameters) {
  juce::ignoreUnused(input, parameters);

  // TODO: deserialize parameters from the JSON input stream

  return juce::Result::fail("not implemented");
}
}  // namespace tremolo
