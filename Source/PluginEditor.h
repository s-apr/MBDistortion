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
    void addSliderVertical(juce::Slider& slider);
    void addComboBox(juce::ComboBox& comboBox);

private:
    void timerCallback() override;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MBDistortionAudioProcessor& audioProcessor;
    OscilloscopeComponent oscilloscope;

    //band sliders 'drive'
    juce::Slider band1Drive, band2Drive, band3Drive, band4Drive;

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

    //other sliders and bypass etc
    juce::Slider inputGainSlider, outputGainSlider, masterMixSlider;
    juce::ToggleButton bypassButton;

    SliderAttachment inputGainSliderAttachment{ audioProcessor.parameters, "inputGain", inputGainSlider };
    SliderAttachment outputGainSliderAttachment{ audioProcessor.parameters, "outputGain", outputGainSlider };
    SliderAttachment masterMixSliderAttachment{ audioProcessor.parameters, "masterMix", masterMixSlider };

    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    ButtonAttachment bypassButtonAttachment{ audioProcessor.parameters, "bypass", bypassButton };

    //crossover sliders
    juce::Slider crossover1Slider, crossover2Slider, crossover3Slider;
    SliderAttachment crossover1Slider1Attachment{ audioProcessor.parameters, "crossoverFreq1", crossover1Slider };
    SliderAttachment crossover2SliderAttachment{ audioProcessor.parameters, "crossoverFreq2", crossover2Slider };
    SliderAttachment crossover3SliderAttachment{ audioProcessor.parameters, "crossoverFreq3", crossover3Slider };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MBDistortionAudioProcessorEditor)
};
