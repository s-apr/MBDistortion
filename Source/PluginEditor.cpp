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

    addAndMakeVisible(oscilloscope);
    setSize(800, 600);

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

    auto bounds = getLocalBounds().reduced(10);
    int H = bounds.getHeight();

    //top rectangle (oscilloscope outline)
    int topH = int(H * 0.25f);
    auto scopeOutline = bounds.removeFromTop(topH);
    g.drawRect(scopeOutline, 2);

    //separator oscilloscope and mid region
    int sepY = scopeOutline.getBottom();
    g.drawLine(float(scopeOutline.getX()), float(sepY),
        float(scopeOutline.getRight()), float(sepY), 1.0f);

    //mid rectangle (for crossover/global controls)
    int midH = int(H * 0.15f);
    auto midOutline = bounds.removeFromTop(midH);
    g.drawRect(midOutline, 2);

    //bottom: four equal columns
    int numCols = 4;
    int colW = bounds.getWidth() / numCols;
    for (int i = 0; i < numCols; ++i)
    {
        auto col = bounds.removeFromLeft(colW);
        g.drawRect(col, 2);
    }
}

void MBDistortionAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    int H = area.getHeight();

    //oscilloscope
    int topH = int(H * 0.25f);
    auto scopeArea = area.removeFromTop(topH);
    oscilloscope.setBounds(scopeArea);

    //remaining 'area' used for mid & bottom controls
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

void MBDistortionAudioProcessorEditor::timerCallback() {
    repaint();
}