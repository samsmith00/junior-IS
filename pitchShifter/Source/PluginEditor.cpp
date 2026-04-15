#pragma once

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
    //pitchShifter.setRange (-12, 12, 1);
    pitchShifter.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    pitchShifter.setPopupDisplayEnabled (true, true, this);
    pitchShifter.setValue(0);
    pitchShifter.addListener(this);

    addAndMakeVisible (pitchShifter);
    pitchShifterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "PS", pitchShifter);

}

void AudioPluginAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    processorRef.pitchFactor = pitchShifter.getValue();
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
    // set the font size and draw text to the screen
    g.setFont (15.0f);
    g.drawFittedText ("Pitch Shifter", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    pitchShifter.setBounds (150, 30, getWidth() / 3, getHeight() / 3);

}


