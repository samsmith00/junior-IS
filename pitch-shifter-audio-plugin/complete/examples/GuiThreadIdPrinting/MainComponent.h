#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <bit>

class GuiThreadIdPrinting : public juce::Component {
public:
  GuiThreadIdPrinting() {
    DBG("Main component constructor thread ID: " << getCurrentThreadId());

    addAndMakeVisible(button);
    button.onClick = []() {
      DBG("Button onClick callback thread ID: " << getCurrentThreadId());
    };

    setSize(400, 200);
  }

  void paint(juce::Graphics&) override {
    DBG("paint() thread ID: " << getCurrentThreadId());
  }

  void resized() override {
    DBG("resized() thread ID: " << getCurrentThreadId());

    button.setBounds(getLocalBounds().reduced(50));
  }

private:
  [[nodiscard]] static juce::String getCurrentThreadId() {
    return juce::String::toHexString(
        std::bit_cast<juce::int64>(juce::Thread::getCurrentThreadId()));
  }

  juce::TextButton button{"Click me!"};
};

using MainComponent = GuiThreadIdPrinting;
