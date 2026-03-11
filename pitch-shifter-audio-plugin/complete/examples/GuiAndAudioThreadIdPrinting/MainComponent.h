#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <bit>

class GuiAndAudioThreadIdPrinting : public juce::AudioAppComponent {
public:
  GuiAndAudioThreadIdPrinting() {
    DBG("Main component constructor thread ID: " << getCurrentThreadId());

    addAndMakeVisible(button);
    button.onClick = []() {
      DBG("Button onClick callback thread ID: " << getCurrentThreadId());
    };

    setSize(400, 200);

    setAudioChannels(2, 2);
  }

  ~GuiAndAudioThreadIdPrinting() { shutdownAudio(); }

  void paint(juce::Graphics&) override {
    DBG("paint() thread ID: " << getCurrentThreadId());
  }

  void resized() override {
    DBG("resized() thread ID: " << getCurrentThreadId());

    button.setBounds(getLocalBounds().reduced(50));
  }

  void prepareToPlay(int expectedFramesPerBlock, double sampleRate) override {
    juce::ignoreUnused(expectedFramesPerBlock, sampleRate);
    DBG("prepareToPlay() thread ID: " << getCurrentThreadId());
  }

  void getNextAudioBlock(const juce::AudioSourceChannelInfo& info) override {
    DBG("getNextAudioBlock() thread ID: " << getCurrentThreadId());

    info.clearActiveBufferRegion();
  }

  void releaseResources() override {
    DBG("releaseResources() thread ID: " << getCurrentThreadId());
  }

private:
  [[nodiscard]] static juce::String getCurrentThreadId() {
    return juce::String::toHexString(
        std::bit_cast<juce::int64>(juce::Thread::getCurrentThreadId()));
  }

  juce::TextButton button{"Click me!"};
};

using MainComponent = GuiAndAudioThreadIdPrinting;
