/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MBDistortionAudioProcessorEditor::MBDistortionAudioProcessorEditor(MBDistortionAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);

    //add sliders
    addSliderRotary(band1Drive);
    addSliderRotary(band2Drive);
    addSliderRotary(band3Drive);
    addSliderRotary(band4Drive);

    //add comboboxes
    addComboBox(band1Selector);
    addComboBox(band2Selector);
    addComboBox(band3Selector);
    addComboBox(band4Selector);

    //attach comboboxes
    //combo boxes must be attatched AFTER being populated not BEFORE
    //this ensures persitience during closing/opening plugin window
    band1TypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.parameters, "band1type", band1Selector);
    band2TypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.parameters, "band2type", band2Selector);
    band3TypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.parameters, "band3type", band3Selector);
    band4TypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.parameters, "band4type", band4Selector);


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
    juce::FlexBox outer;
    outer.flexDirection = juce::FlexBox::Direction::column;
    outer.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    //sliders
    juce::FlexBox sliderRow;
    sliderRow.flexDirection = juce::FlexBox::Direction::row;
    sliderRow.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

    sliderRow.items.add(juce::FlexItem(band1Drive).withFlex(1.0f).withMinWidth(80));
    sliderRow.items.add(juce::FlexItem(band2Drive).withFlex(1.0f).withMinWidth(80));
    sliderRow.items.add(juce::FlexItem(band3Drive).withFlex(1.0f).withMinWidth(80));
    sliderRow.items.add(juce::FlexItem(band4Drive).withFlex(1.0f).withMinWidth(80));

    //combo box
    juce::FlexBox selectorRow;
    selectorRow.flexDirection = juce::FlexBox::Direction::row;
    selectorRow.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

    selectorRow.items.add(juce::FlexItem(band1Selector).withFlex(1.0f).withMinWidth(80));
    selectorRow.items.add(juce::FlexItem(band2Selector).withFlex(1.0f).withMinWidth(80));
    selectorRow.items.add(juce::FlexItem(band3Selector).withFlex(1.0f).withMinWidth(80));
    selectorRow.items.add(juce::FlexItem(band4Selector).withFlex(1.0f).withMinWidth(80));

    //layout
    outer.items.add(juce::FlexItem(sliderRow).withFlex(3.0f));
    outer.items.add(juce::FlexItem(selectorRow).withFlex(0.5f));
    outer.performLayout(getLocalBounds().reduced(10));

}

void MBDistortionAudioProcessorEditor::addSliderRotary(juce::Slider& slider) {
    slider.setSliderStyle(juce::Slider::Rotary);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    slider.setNumDecimalPlacesToDisplay(2);
    addAndMakeVisible(&slider);
}

void MBDistortionAudioProcessorEditor::addComboBox(juce::ComboBox& comboBox) {

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