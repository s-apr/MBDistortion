/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "OscilloscopeComponent.h"

//==============================================================================
/**
*/
class MBDistortionAudioProcessorEditor : public juce::AudioProcessorEditor,
    juce::Timer
{
public:
    MBDistortionAudioProcessorEditor (MBDistortionAudioProcessor&);
    ~MBDistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void addSliderRotary(juce::Slider& slider);
    void addComboBox(juce::ComboBox& comboBox);

private:
    void timerCallback() override;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MBDistortionAudioProcessor& audioProcessor;
    OscilloscopeComponent oscilloscope;

    //band sliders 'drive'
    juce::Slider band1Drive;
    juce::Slider band2Drive;
    juce::Slider band3Drive;
    juce::Slider band4Drive;

    //band sliders attachmnents
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    SliderAttachment band1DriveAttachment{ audioProcessor.parameters, "band1drive", band1Drive };
    SliderAttachment band2DriveAttachment{ audioProcessor.parameters, "band2drive", band2Drive };
    SliderAttachment band3DriveAttachment{ audioProcessor.parameters, "band3drive", band3Drive };
    SliderAttachment band4DriveAttachment{ audioProcessor.parameters, "band4drive", band4Drive };

    //distortion selectors
    juce::ComboBox band1Selector, band2Selector, band3Selector, band4Selector;

    //distortion selector attachments
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<ComboBoxAttachment> band1TypeAttachment;
    std::unique_ptr<ComboBoxAttachment> band2TypeAttachment;
    std::unique_ptr<ComboBoxAttachment> band3TypeAttachment;
    std::unique_ptr<ComboBoxAttachment> band4TypeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MBDistortionAudioProcessorEditor)
};
