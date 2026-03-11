#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class UserEventHandlers : public juce::Component {
public:
  UserEventHandlers() {
    setBounds(0, 0, 400, 300);

    addAndMakeVisible(button);
    button.onClick = []() { jassertfalse; };
  }

  void resized() override { button.setBounds(getLocalBounds().reduced(50)); }

private:
  juce::TextButton button{"Click me to pause the debugger"};
};

using MainComponent = UserEventHandlers;
