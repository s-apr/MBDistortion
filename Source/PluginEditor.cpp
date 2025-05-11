/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MBDistortionAudioProcessorEditor::MBDistortionAudioProcessorEditor(MBDistortionAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), oscilloscope(p.oscBuffer)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    audioProcessor.oscBuffer.resize(int(audioProcessor.getSampleRate() * 0.1));

    //band drive sliders
    addSliderRotary(band1Drive);
    addSliderRotary(band2Drive);
    addSliderRotary(band3Drive);
    addSliderRotary(band4Drive);

    //add comboboxes
    addTypeComboBox(band1Selector);
    addTypeComboBox(band2Selector);
    addTypeComboBox(band3Selector);
    addTypeComboBox(band4Selector);

    //add oversample selector
    addFactorComboBox(oversampleSelector);

    //attach comboboxes
    //combo boxes must be attatched AFTER being populated not BEFORE
    //this ensures persitience during closing/opening plugin window
    band1TypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.parameters, "band1type", band1Selector);
    band2TypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.parameters, "band2type", band2Selector);
    band3TypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.parameters, "band3type", band3Selector);
    band4TypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.parameters, "band4type", band4Selector);

    //oversample
    oversampleSelectorAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.parameters, "oversamplingFactor", oversampleSelector);

    addAndMakeVisible(oscilloscope);

    //global controls
    addSliderRotary(inputGainSlider);
    addSliderRotary(outputGainSlider);
    addSliderRotary(masterMixSlider);
    addAndMakeVisible(bypassButton);

    //slider crossovver
    addSliderHorizontal(crossover1Slider);
    addSliderHorizontal(crossover2Slider);
    addSliderHorizontal(crossover3Slider);

    //band level
    addSliderVertical(band1Level);
    addSliderVertical(band2Level);
    addSliderVertical(band3Level);
    addSliderVertical(band4Level);

    //mute
    addAndMakeVisible(band1MuteButton);
    addAndMakeVisible(band2MuteButton);
    addAndMakeVisible(band3MuteButton);
    addAndMakeVisible(band4MuteButton);
    band1MuteButton.setButtonText("Mute");
    band2MuteButton.setButtonText("Mute");
    band3MuteButton.setButtonText("Mute");
    band4MuteButton.setButtonText("Mute");
    
    //solo
    addAndMakeVisible(band1SoloButton);
    addAndMakeVisible(band2SoloButton);
    addAndMakeVisible(band3SoloButton);
    addAndMakeVisible(band4SoloButton);
    band1SoloButton.setButtonText("Solo");
    band2SoloButton.setButtonText("Solo");
    band3SoloButton.setButtonText("Solo");
    band4SoloButton.setButtonText("Solo");

    setSize(1000, 800);

    startTimerHz(60);

}

MBDistortionAudioProcessorEditor::~MBDistortionAudioProcessorEditor()
{
}

//==============================================================================
void MBDistortionAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);

    auto area = getLocalBounds().reduced(10);
    int height = area.getHeight();

    //top rectangle (oscilloscope outline)
    auto topHeight = int(height * 0.25);
    auto scopeArea = area.removeFromTop(topHeight);
    g.drawRect(scopeArea, 1);
    oscilloscope.setBounds(scopeArea.reduced(5));

    //toplow
    int topmidHeight = int(height * 0.1f);
    auto crossovers = area.removeFromTop(topmidHeight);
    g.drawRect(crossovers, 1);

    //middle
    auto globalHeight = int(height * 0.15);
    auto globalArea = area.removeFromTop(globalHeight);
    g.drawRect(globalArea, 1);

    //band settings (splitting it up I think)
    auto totalBandHeight = int(height * 0.5);
    auto bandWidth = getWidth() / 4;
    auto bandArea = area.removeFromTop(totalBandHeight);

    auto band1Area = bandArea.removeFromLeft(bandWidth);
    auto band2Area = bandArea.removeFromLeft(bandWidth);
    auto band3Area = bandArea.removeFromLeft(bandWidth);
    auto band4Area = bandArea.removeFromLeft(bandWidth);
    g.drawRect(band1Area, 1);
    g.drawRect(band2Area, 1);
    g.drawRect(band3Area, 1);
    g.drawRect(band4Area, 1);

    //char curves
    auto charCurve1 = band1Area.removeFromTop(totalBandHeight * 0.4);
    g.drawRect(charCurve1, 1);
    auto charCurve2 = band2Area.removeFromTop(totalBandHeight * 0.4);
    g.drawRect(charCurve2, 1);
    auto charCurve3 = band3Area.removeFromTop(totalBandHeight * 0.4);
    g.drawRect(charCurve3, 1);
    auto charCurve4 = band4Area.removeFromTop(totalBandHeight * 0.4);
    g.drawRect(charCurve4, 1);

    //drive area
    //drive knob (left), band volume (slider, left)
    //mute / solo bottom
    auto driveArea1 = band1Area.removeFromTop(totalBandHeight * 0.4);
    g.drawRect(driveArea1, 1);
    auto bandVolumeArea1 = driveArea1.removeFromLeft(bandWidth * 0.75);
    g.drawRect(bandVolumeArea1, 1);

    auto driveArea2 = band2Area.removeFromTop(totalBandHeight * 0.4);
    g.drawRect(driveArea2, 1);
    auto bandVolumeArea2 = driveArea2.removeFromLeft(bandWidth * 0.75);
    g.drawRect(bandVolumeArea2, 1);

    auto driveArea3 = band3Area.removeFromTop(totalBandHeight * 0.4);
    g.drawRect(driveArea3, 1);
    auto bandVolumeArea3 = driveArea3.removeFromLeft(bandWidth * 0.75);
    g.drawRect(bandVolumeArea3, 1);

    auto driveArea4 = band4Area.removeFromTop(totalBandHeight * 0.4);
    g.drawRect(driveArea4, 1);
    auto bandVolumeArea4 = driveArea4.removeFromLeft(bandWidth * 0.75);
    g.drawRect(bandVolumeArea4, 1);

    //combo boxes
    auto comboboxArea1 = band1Area.removeFromTop(totalBandHeight * 0.1);
    g.drawRect(comboboxArea1, 1);
    auto comboboxArea2 = band2Area.removeFromTop(totalBandHeight * 0.1);
    g.drawRect(comboboxArea2, 1);
    auto comboboxArea3 = band3Area.removeFromTop(totalBandHeight * 0.1);
    g.drawRect(comboboxArea3, 1);
    auto comboboxArea4 = band4Area.removeFromTop(totalBandHeight * 0.1);
    g.drawRect(comboboxArea4, 1);

    //mute, solo
    auto muteArea1 = band1Area.removeFromLeft(bandWidth / 2);
    auto muteArea2 = band2Area.removeFromLeft(bandWidth / 2);
    auto muteArea3 = band3Area.removeFromLeft(bandWidth / 2);
    auto muteArea4 = band4Area.removeFromLeft(bandWidth / 2);


}

void MBDistortionAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto height = area.getHeight();

    //top rectangle (oscilloscope)
    auto topHeight = int(height * 0.25);
    auto scopeArea = area.removeFromTop(topHeight);

    //crossover sliders
    auto crossoverHeight = int(height * 0.1);
    auto crossoverArea = area.removeFromTop(crossoverHeight);
    crossover1Slider.setBounds(crossoverArea.removeFromLeft(getWidth()/3).reduced(5));
    crossover2Slider.setBounds(crossoverArea.removeFromLeft(getWidth()/3).reduced(5));
    crossover3Slider.setBounds(crossoverArea.removeFromLeft(getWidth()/3).reduced(5));

    //global controls
    auto globalHeight = int(height * 0.15);
    auto globalArea = area.removeFromTop(globalHeight);
    inputGainSlider.setBounds(globalArea.removeFromLeft(getWidth()/5).reduced(5));
    masterMixSlider.setBounds(globalArea.removeFromLeft(getWidth()/5).reduced(5));
    outputGainSlider.setBounds(globalArea.removeFromLeft(getWidth()/5).reduced(5));
    bypassButton.setBounds(globalArea.removeFromLeft(getWidth()/5).reduced(5));
    oversampleSelector.setBounds(globalArea.removeFromLeft(getWidth()/5).reduced(5));

    //ALL BAND SETTINGS
    auto totalBandHeight = int(height * 0.5);
    auto bandWidth = getWidth() / 4;
    auto bandArea = area.removeFromTop(totalBandHeight);

    auto band1Area = bandArea.removeFromLeft(bandWidth);
    auto band2Area = bandArea.removeFromLeft(bandWidth);
    auto band3Area = bandArea.removeFromLeft(bandWidth);
    auto band4Area = bandArea.removeFromLeft(bandWidth);

    //char curve
    auto charCurve1 = band1Area.removeFromTop(totalBandHeight * 0.4);
    auto charCurve2 = band2Area.removeFromTop(totalBandHeight * 0.4);
    auto charCurve3 = band3Area.removeFromTop(totalBandHeight * 0.4);
    auto charCurve4 = band4Area.removeFromTop(totalBandHeight * 0.4);
    
    //drive1
    auto driveArea1 = band1Area.removeFromTop(totalBandHeight * 0.4);
    band1Drive.setBounds(driveArea1.removeFromLeft(bandWidth * 0.75).reduced(5));
    //level1
    auto bandVolumeArea1 = driveArea1.removeFromLeft(bandWidth * 0.4);
    band1Level.setBounds(bandVolumeArea1.reduced(5));

    //drive2
    auto driveArea2 = band2Area.removeFromTop(totalBandHeight * 0.4);
    band2Drive.setBounds(driveArea2.removeFromLeft(bandWidth * 0.75).reduced(5));
    //level2
    auto bandVolumeArea2 = driveArea2.removeFromLeft(bandWidth * 0.4);
    band2Level.setBounds(bandVolumeArea2.reduced(5));

    //drive3
    auto driveArea3 = band3Area.removeFromTop(totalBandHeight * 0.4);
    band3Drive.setBounds(driveArea3.removeFromLeft(bandWidth * 0.75).reduced(5));
    //level3
    auto bandVolumeArea3 = driveArea3.removeFromLeft(bandWidth * 0.4);
    band3Level.setBounds(bandVolumeArea3.reduced(5));

    //drive4
    auto driveArea4 = band4Area.removeFromTop(totalBandHeight * 0.4);
    band4Drive.setBounds(driveArea4.removeFromLeft(bandWidth * 0.75).reduced(5));
    //level4
    auto bandVolumeArea4 = driveArea4.removeFromLeft(bandWidth * 0.4);
    band4Level.setBounds(bandVolumeArea4.reduced(5));

    //combo boxes
    auto comboboxArea1 = band1Area.removeFromTop(totalBandHeight * 0.1);
    band1Selector.setBounds(comboboxArea1.reduced(5));
    auto comboboxArea2 = band2Area.removeFromTop(totalBandHeight * 0.1);
    band2Selector.setBounds(comboboxArea2.reduced(5));
    auto comboboxArea3 = band3Area.removeFromTop(totalBandHeight * 0.1);
    band3Selector.setBounds(comboboxArea3.reduced(5));
    auto comboboxArea4 = band4Area.removeFromTop(totalBandHeight * 0.1);
    band4Selector.setBounds(comboboxArea4.reduced(5));

    //mute, solo
    auto muteArea1 = band1Area.removeFromLeft(bandWidth / 2);
    auto soloArea1 = band1Area.removeFromLeft(bandWidth / 2);
    band1MuteButton.setBounds(muteArea1.reduced(5));
    band1SoloButton.setBounds(soloArea1.reduced(5));

    auto muteArea2 = band2Area.removeFromLeft(bandWidth / 2);
    auto soloArea2 = band2Area.removeFromLeft(bandWidth / 2);
    band2MuteButton.setBounds(muteArea2.reduced(5));
    band2SoloButton.setBounds(soloArea2.reduced(5));

    auto muteArea3 = band3Area.removeFromLeft(bandWidth / 2);
    auto soloArea3 = band3Area.removeFromLeft(bandWidth / 2);
    band3MuteButton.setBounds(muteArea3.reduced(5));
    band3SoloButton.setBounds(soloArea3.reduced(5));

    auto muteArea4 = band4Area.removeFromLeft(bandWidth / 2);
    auto soloArea4 = band4Area.removeFromLeft(bandWidth / 2);
    band4MuteButton.setBounds(muteArea4.reduced(5));
    band4SoloButton.setBounds(soloArea4.reduced(5));
}

void MBDistortionAudioProcessorEditor::addSliderRotary(juce::Slider& slider) {
    slider.setSliderStyle(juce::Slider::Rotary);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    slider.setNumDecimalPlacesToDisplay(2);
    addAndMakeVisible(&slider);
}

void MBDistortionAudioProcessorEditor::addSliderVertical(juce::Slider& slider) {
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    slider.setNumDecimalPlacesToDisplay(2);
    addAndMakeVisible(&slider);
}

void MBDistortionAudioProcessorEditor::addSliderHorizontal(juce::Slider& slider) {
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    slider.setNumDecimalPlacesToDisplay(2);
    addAndMakeVisible(&slider);
}

void MBDistortionAudioProcessorEditor::addTypeComboBox(juce::ComboBox& comboBox) {

    comboBox.addItem("None", 1);
    comboBox.addItem("Hard Clip", 2);
    comboBox.addItem("Soft Clip", 3);
    comboBox.addItem("Exponential Distortion", 4);
    comboBox.addItem("Cubic Clip", 5);
    comboBox.addItem("Arctangent", 6);
    comboBox.addItem("Asymmetric", 7);
    comboBox.addItem("Full Rectify", 8);
    comboBox.addItem("Half Rectify", 9);

    addAndMakeVisible(comboBox);
}


void MBDistortionAudioProcessorEditor::addFactorComboBox(juce::ComboBox& comboBox) {

    comboBox.addItem("Off", 1);
    comboBox.addItem("2x", 2);
    comboBox.addItem("4x", 3);
    comboBox.addItem("8x", 4);

    addAndMakeVisible(comboBox);
}

void MBDistortionAudioProcessorEditor::timerCallback() {
    repaint();
}