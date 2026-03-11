#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>

#define LABEL_WITH_JUSTIFICATION(justificationType)                       \
  LabelWithJustification {                                                \
    .label = {#justificationType, #justificationType}, .justification = { \
      juce::Justification::justificationType                              \
    }                                                                     \
  }

class Justifications : public juce::Component {
public:
  Justifications() {
    for (auto& label : labels) {
      label.label.setJustificationType(label.justification);
      addAndMakeVisible(label.label);
    }

    setSize(500, 500);
  }

  void paint(juce::Graphics& g) override { g.drawRect(getLabelBounds(), 2); }

  void resized() override {
    const auto bounds = getLabelBounds();

    for (auto& label : labels) {
      label.label.setBounds(bounds);
    }
  }

private:
  juce::Rectangle<int> getLabelBounds() { return getLocalBounds().reduced(20); }

  struct LabelWithJustification {
    juce::Label label;
    juce::Justification justification;
  };

  // Commented out justifications conflict with the rest
  std::array<LabelWithJustification, 9> labels{
      // LABEL_WITH_JUSTIFICATION(left),
      // LABEL_WITH_JUSTIFICATION(right),
      // LABEL_WITH_JUSTIFICATION(horizontallyCentred),
      // LABEL_WITH_JUSTIFICATION(top),
      // LABEL_WITH_JUSTIFICATION(bottom),
      // LABEL_WITH_JUSTIFICATION(verticallyCentred),
      // LABEL_WITH_JUSTIFICATION(horizontallyJustified),
      LABEL_WITH_JUSTIFICATION(centred),
      LABEL_WITH_JUSTIFICATION(centredLeft),
      LABEL_WITH_JUSTIFICATION(centredRight),
      LABEL_WITH_JUSTIFICATION(centredTop),
      LABEL_WITH_JUSTIFICATION(centredBottom),
      LABEL_WITH_JUSTIFICATION(topLeft),
      LABEL_WITH_JUSTIFICATION(topRight),
      LABEL_WITH_JUSTIFICATION(bottomLeft),
      LABEL_WITH_JUSTIFICATION(bottomRight),
  };
};

#undef LABEL_WITH_JUSTIFICATION

using MainComponent = Justifications;
