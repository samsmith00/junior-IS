#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    pitchShifter.setSliderStyle (juce::Slider::SliderStyle::Rotary);
    pitchShifter.setRange (-5, 5, 1);
    pitchShifter.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    pitchShifter.setPopupDisplayEnabled (true, true, this);
    //pitchFactor.setTextValueSuffix ("Pitch Shift");
    pitchShifter.setValue(0);
    pitchShifter.addListener(this);

    addAndMakeVisible (pitchShifter);


}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
