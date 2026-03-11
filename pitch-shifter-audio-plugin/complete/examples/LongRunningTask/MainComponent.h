#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>

class LongRunningTask : public juce::Component {
public:
  LongRunningTask() {
    startButton.onClick = [this]() { findLargestPrimeNumberWithinLimit(); };
    addAndMakeVisible(startButton);

    addAndMakeVisible(progressBar);

    resultLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(resultLabel);

    setSize(500, 300);
  }

  void resized() override {
    auto bounds = getLocalBounds();

    startButton.setBounds(bounds.removeFromTop(100).reduced(20));
    progressBar.setBounds(bounds.removeFromTop(100).reduced(20));
    resultLabel.setBounds(bounds.removeFromTop(100).reduced(20));
  }

private:
  void findLargestPrimeNumberWithinLimit() {
    int largestPrimeNumberFound = 2;

    constexpr auto limit = 10'000'000;

    for (int currentExaminedNumber = 3; currentExaminedNumber < limit;
         currentExaminedNumber += 2) {
      auto isPrime = true;

      for (long i = 3; i <= std::sqrt(currentExaminedNumber); i++) {
        if (currentExaminedNumber % i == 0) {
          isPrime = false;
          break;
        }
      }

      if (isPrime) {
        largestPrimeNumberFound = currentExaminedNumber;
      }

      progress = std::clamp(static_cast<double>(currentExaminedNumber) /
                                static_cast<double>(limit),
                            0.0, 1.0);
    }

    progress = 1.0;
    resultLabel.setText("Largest prime number < " + juce::String{limit} +
                            " found: " + juce::String{largestPrimeNumberFound},
                        juce::dontSendNotification);
  }

  juce::TextButton startButton{"Click to start looking for prime numbers"};
  double progress{0.0};
  juce::ProgressBar progressBar{progress, juce::ProgressBar::Style::linear};
  juce::Label resultLabel{"result label", "The result should appear here"};
};

using MainComponent = LongRunningTask;
