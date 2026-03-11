#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>

class LongRunningTask : public juce::Component, private juce::Thread {
public:
  LongRunningTask() : juce::Thread{"LongRunningTask"} {
    startButton.onClick = [this]() {
      if (startThread()) {
        progress = 0.0;
      }
    };
    addAndMakeVisible(startButton);

    addAndMakeVisible(progressBar);

    resultLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(resultLabel);

    setSize(500, 300);
  }

  ~LongRunningTask() override { stopThread(100); }

  void resized() override {
    auto bounds = getLocalBounds();

    startButton.setBounds(bounds.removeFromTop(100).reduced(20));
    progressBar.setBounds(bounds.removeFromTop(100).reduced(20));
    resultLabel.setBounds(bounds.removeFromTop(100).reduced(20));
  }

private:
  static constexpr auto limit = 10'000'000;

  void findLargestPrimeNumberWithinLimit() {
    auto progressPercent = 0;
    constexpr auto progressMultiplier = 100.0 / limit;

    auto largestPrimeNumberFound = 2;

    for (int currentExaminedNumber = 3; currentExaminedNumber < limit;
         currentExaminedNumber += 2) {
      if (threadShouldExit()) {
        return;
      }

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

      const auto newProgressPercent =
          static_cast<int>(currentExaminedNumber * progressMultiplier);
      if (progressPercent < newProgressPercent) {
        progressPercent = newProgressPercent;
        const auto newProgress = juce::jmap(
            static_cast<double>(progressPercent), 0.0, 100.0, 0.0, 1.0);
        juce::MessageManager::callAsync(
            [this, newProgress]() { progress = newProgress; });
      }
    }

    juce::MessageManager::callAsync([this, largestPrimeNumberFound]() {
      progress = 1.0;
      resultLabel.setText(
          "Largest prime number < " + juce::String{limit} +
              " found: " + juce::String{largestPrimeNumberFound},
          juce::dontSendNotification);
    });
  }

  void run() override { findLargestPrimeNumberWithinLimit(); }

  juce::TextButton startButton{"Click to start looking for prime numbers"};
  double progress{0.0};
  juce::ProgressBar progressBar{progress, juce::ProgressBar::Style::linear};
  juce::Label resultLabel{"result label", "The result should appear here"};
};

using MainComponent = LongRunningTask;
