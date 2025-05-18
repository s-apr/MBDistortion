/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "OscilloscopeComponent.h"
#include "CustomLookAndFeel.h"

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
    void addSliderHorizontal(juce::Slider& slider);
    void addTypeComboBox(juce::ComboBox& comboBox);
    void addFactorComboBox(juce::ComboBox& comboBox);

private:

    void timerCallback() override;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MBDistortionAudioProcessor& audioProcessor;
    OscilloscopeComponent oscilloscope;

    //band sliders 'drive'
    juce::Slider band1Drive, band2Drive, band3Drive, band4Drive;
    juce::Label band1DriveLabel, band2DriveLabel, band3DriveLabel, band4DriveLabel;

    //band sliders attachmnents
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    SliderAttachment band1DriveAttachment{ audioProcessor.parameters, "band1drive", band1Drive };
    SliderAttachment band2DriveAttachment{ audioProcessor.parameters, "band2drive", band2Drive };
    SliderAttachment band3DriveAttachment{ audioProcessor.parameters, "band3drive", band3Drive };
    SliderAttachment band4DriveAttachment{ audioProcessor.parameters, "band4drive", band4Drive };

    //distortion selectors
    juce::ComboBox band1Selector, band2Selector, band3Selector, band4Selector;
    juce::Label band1SelectorLabel, band2SelectorLabel, band3SelectorLabel, band4SelectorLabel;

    //distortion selector attachments
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<ComboBoxAttachment> band1TypeAttachment;
    std::unique_ptr<ComboBoxAttachment> band2TypeAttachment;
    std::unique_ptr<ComboBoxAttachment> band3TypeAttachment;
    std::unique_ptr<ComboBoxAttachment> band4TypeAttachment;

    //other sliders and bypass etc
    juce::Slider inputGainSlider, outputGainSlider, masterMixSlider;
    juce::Label inputGainLabel, outputGainLabel, masterMixLabel;
    juce::ToggleButton bypassButton;
    juce::Label bypassLabel;

    SliderAttachment inputGainSliderAttachment{ audioProcessor.parameters, "inputGain", inputGainSlider };
    SliderAttachment outputGainSliderAttachment{ audioProcessor.parameters, "outputGain", outputGainSlider };
    SliderAttachment masterMixSliderAttachment{ audioProcessor.parameters, "masterMix", masterMixSlider };

    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    ButtonAttachment bypassButtonAttachment{ audioProcessor.parameters, "bypass", bypassButton };

    //crossover sliders
    juce::Slider crossover1Slider, crossover2Slider, crossover3Slider;
    juce::Label crossover1Label, crossover2Label, crossover3Label;
    SliderAttachment crossover1Slider1Attachment{ audioProcessor.parameters, "crossoverFreq1", crossover1Slider };
    SliderAttachment crossover2SliderAttachment{ audioProcessor.parameters, "crossoverFreq2", crossover2Slider };
    SliderAttachment crossover3SliderAttachment{ audioProcessor.parameters, "crossoverFreq3", crossover3Slider };

    //band levels
    juce::Slider band1Level, band2Level, band3Level, band4Level;
    juce::Label band1LevelLabel, band2LevelLabel, band3LevelLabel, band4LevelLabel;
    SliderAttachment band1LevelAttachment{ audioProcessor.parameters, "band1level", band1Level };
    SliderAttachment band2LevelAttachment{ audioProcessor.parameters, "band2level", band2Level };
    SliderAttachment band3LevelAttachment{ audioProcessor.parameters, "band3level", band3Level };
    SliderAttachment band4LevelAttachment{ audioProcessor.parameters, "band4level", band4Level };

    //mute,solo
    juce::ToggleButton band1MuteButton, band2MuteButton, band3MuteButton, band4MuteButton;
    juce::ToggleButton band1SoloButton, band2SoloButton, band3SoloButton, band4SoloButton;
    ButtonAttachment band1MuteButtonAttachment{ audioProcessor.parameters, "band1mute", band1MuteButton };
    ButtonAttachment band1SoloButtonAttachment{ audioProcessor.parameters, "band1solo", band1SoloButton };
    ButtonAttachment band2MuteButtonAttachment{ audioProcessor.parameters, "band2mute", band2MuteButton };
    ButtonAttachment band2SoloButtonAttachment{ audioProcessor.parameters, "band2solo", band2SoloButton };
    ButtonAttachment band3MuteButtonAttachment{ audioProcessor.parameters, "band3mute", band3MuteButton };
    ButtonAttachment band3SoloButtonAttachment{ audioProcessor.parameters, "band3solo", band3SoloButton };
    ButtonAttachment band4MuteButtonAttachment{ audioProcessor.parameters, "band4mute", band4MuteButton };
    ButtonAttachment band4SoloButtonAttachment{ audioProcessor.parameters, "band4solo", band4SoloButton };

    //oversample selector
    juce::ComboBox oversampleSelector;
    juce::Label oversampleLabel;
    std::unique_ptr<ComboBoxAttachment> oversampleSelectorAttachment;

    //characteristic curve dispaly
    void drawCharacteristicCurve(juce::Graphics& g, juce::Rectangle<int> bounds,
        DistortionTypes type, float drive, float level);

    //custom look and feel
    CustomLookAndFeel customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MBDistortionAudioProcessorEditor)
};
