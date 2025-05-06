/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MBDistortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MBDistortionAudioProcessorEditor (MBDistortionAudioProcessor&);
    ~MBDistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void addSliderRotary(juce::Slider& slider);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MBDistortionAudioProcessor& audioProcessor;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MBDistortionAudioProcessorEditor)
};
