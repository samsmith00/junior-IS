#pragma once
#include <string>
#include <juce_core/juce_core.h>

namespace tremolo {
inline std::string getFileOutputPath(juce::StringRef fileName) {
  return juce::File::getSpecialLocation(
             juce::File::SpecialLocationType::currentExecutableFile)
      .getParentDirectory()
      .getChildFile(fileName)
      .getFullPathName()
      .toStdString();
}
}  // namespace tremolo
