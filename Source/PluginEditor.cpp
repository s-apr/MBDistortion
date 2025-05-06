/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MBDistortionAudioProcessorEditor::MBDistortionAudioProcessorEditor (MBDistortionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300); 

    //add sliders
    addSliderRotary(band1Drive);
    addSliderRotary(band2Drive);
    addSliderRotary(band3Drive);
    addSliderRotary(band4Drive);

}

MBDistortionAudioProcessorEditor::~MBDistortionAudioProcessorEditor()
{
}

//==============================================================================
void MBDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void MBDistortionAudioProcessorEditor::resized()
{
    //flexbox
}

void MBDistortionAudioProcessorEditor::addSliderRotary(juce::Slider& slider) {
    slider.setSliderStyle(juce::Slider::Rotary);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    slider.setNumDecimalPlacesToDisplay(2);
    addAndMakeVisible(&slider);
}