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

    setLookAndFeel(&customLookAndFeel);

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
    setLookAndFeel(nullptr);
}

//==============================================================================
void MBDistortionAudioProcessorEditor::paint(juce::Graphics& g)
{
    //colours
    auto baseColour = juce::Colour(0xff2e2e2e);
    auto panelColour = juce::Colour(0xff323940);
    auto bandColumnColour = juce::Colour(0xff323940);

    //background
    g.fillAll(baseColour);

    //padding, bounds, ara, etc
    const int padding = 8;
    auto bounds = getLocalBounds(); 
    auto area = bounds.reduced(padding); 
    auto initialArea = area;

    int availableHeight = area.getHeight();

    //section background

    //oscilloscope 
    auto topHeight = int(availableHeight * 0.25);
    auto scopeSectionArea = area.removeFromTop(topHeight);
    g.setColour(panelColour);
    g.fillRect(scopeSectionArea);

    //gap
    area.removeFromTop(padding / 2);

    //crossovers
    auto crossoverHeight = int(availableHeight * 0.1f);
    auto crossoverSectionArea = area.removeFromTop(crossoverHeight);
    g.setColour(panelColour);
    g.fillRect(crossoverSectionArea);

    //gap
    area.removeFromTop(padding / 2);

    //global controls
    auto globalHeight = int(availableHeight * 0.15);
    auto globalSectionArea = area.removeFromTop(globalHeight);
    g.setColour(panelColour);
    g.fillRect(globalSectionArea);

    //gap
    area.removeFromTop(padding / 2);

    //band area
    auto bandSectionArea = area;
    g.setColour(bandColumnColour);
    g.fillRect(bandSectionArea);

    //characteristic curves
    auto totalBandAreaHeight = int(initialArea.getHeight() * 0.5);
    auto bandWidth = initialArea.getWidth() / 4;

    //parameters
    float band1Drive = *audioProcessor.parameters.getRawParameterValue("band1drive");
    float band1Level = *audioProcessor.parameters.getRawParameterValue("band1level");
    float band1TypeFloat = *audioProcessor.parameters.getRawParameterValue("band1type");

    float band2Drive = *audioProcessor.parameters.getRawParameterValue("band2drive");
    float band2Level = *audioProcessor.parameters.getRawParameterValue("band2level");
    float band2TypeFloat = *audioProcessor.parameters.getRawParameterValue("band2type");

    float band3Drive = *audioProcessor.parameters.getRawParameterValue("band3drive");
    float band3Level = *audioProcessor.parameters.getRawParameterValue("band3level");
    float band3TypeFloat = *audioProcessor.parameters.getRawParameterValue("band3type");

    float band4Drive = *audioProcessor.parameters.getRawParameterValue("band4drive");
    float band4Level = *audioProcessor.parameters.getRawParameterValue("band4level");
    float band4TypeFloat = *audioProcessor.parameters.getRawParameterValue("band4type");

    int bandSectionY = bandSectionArea.getY();

    auto charCurveHeight = int(totalBandAreaHeight * 0.4);
    auto curvePadding = 4;

    juce::Rectangle<int> charCurve1Bounds(bandSectionArea.getX(), bandSectionY, bandWidth, charCurveHeight);
    juce::Rectangle<int> charCurve2Bounds(bandSectionArea.getX() + bandWidth, bandSectionY, bandWidth, charCurveHeight);
    juce::Rectangle<int> charCurve3Bounds(bandSectionArea.getX() + bandWidth * 2, bandSectionY, bandWidth, charCurveHeight);
    juce::Rectangle<int> charCurve4Bounds(bandSectionArea.getX() + bandWidth * 3, bandSectionY, bandWidth, charCurveHeight);

    //draw curves
    drawCharacteristicCurve(g, charCurve1Bounds.reduced(curvePadding), static_cast<DistortionTypes>(static_cast<int>(band1TypeFloat)), band1Drive, band1Level);
    drawCharacteristicCurve(g, charCurve2Bounds.reduced(curvePadding), static_cast<DistortionTypes>(static_cast<int>(band2TypeFloat)), band2Drive, band2Level);
    drawCharacteristicCurve(g, charCurve3Bounds.reduced(curvePadding), static_cast<DistortionTypes>(static_cast<int>(band3TypeFloat)), band3Drive, band3Level);
    drawCharacteristicCurve(g, charCurve4Bounds.reduced(curvePadding), static_cast<DistortionTypes>(static_cast<int>(band4TypeFloat)), band4Drive, band4Level);

}

void MBDistortionAudioProcessorEditor::resized()
{
    //main properties
    const int padding = 8; 
    auto bounds = getLocalBounds();
    auto area = bounds.reduced(padding);
    auto initialArea = area;

    int availableHeight = area.getHeight();

    //oscilloscope
    auto topHeight = int(availableHeight * 0.25);
    auto scopeArea = area.removeFromTop(topHeight);
    oscilloscope.setBounds(scopeArea.reduced(padding / 2));

    //gap
    area.removeFromTop(padding / 2);

    //crossovers
    auto crossoverHeight = int(availableHeight * 0.1);
    auto crossoverArea = area.removeFromTop(crossoverHeight);
    auto crossoverSliderWidth = crossoverArea.getWidth() / 3;
    crossover1Slider.setBounds(crossoverArea.removeFromLeft(crossoverSliderWidth).reduced(padding / 2));
    crossover2Slider.setBounds(crossoverArea.removeFromLeft(crossoverSliderWidth).reduced(padding / 2));
    crossover3Slider.setBounds(crossoverArea.removeFromLeft(crossoverSliderWidth).reduced(padding / 2));

    //gap
    area.removeFromTop(padding / 2);

    //global controls
    auto globalHeight = int(availableHeight * 0.15);
    auto globalArea = area.removeFromTop(globalHeight);
    auto globalCtrlWidth = globalArea.getWidth() / 5; // 5 global controls
    inputGainSlider.setBounds(globalArea.removeFromLeft(globalCtrlWidth).reduced(padding / 2));
    masterMixSlider.setBounds(globalArea.removeFromLeft(globalCtrlWidth).reduced(padding / 2));
    outputGainSlider.setBounds(globalArea.removeFromLeft(globalCtrlWidth).reduced(padding / 2));
    bypassButton.setBounds(globalArea.removeFromLeft(globalCtrlWidth).reduced(padding)); // Buttons might need more padding
    oversampleSelector.setBounds(globalArea.removeFromLeft(globalCtrlWidth).reduced(padding / 2));

    //gap
    area.removeFromTop(padding / 2);

    //band settings
    auto bandSectionArea = area; 
    auto totalBandAreaHeight = bandSectionArea.getHeight();
    auto bandWidth = bandSectionArea.getWidth() / 4;

    //columns
    auto band1ColArea = bandSectionArea.removeFromLeft(bandWidth);
    auto band2ColArea = bandSectionArea.removeFromLeft(bandWidth);
    auto band3ColArea = bandSectionArea.removeFromLeft(bandWidth);
    auto band4ColArea = bandSectionArea; 

    auto band1CtrlArea = band1ColArea;
    auto band2CtrlArea = band2ColArea;
    auto band3CtrlArea = band3ColArea;
    auto band4CtrlArea = band4ColArea;

    //skip char curves (handled in painting)
    auto charCurveHeight = int(totalBandAreaHeight * 0.4);
    band1CtrlArea.removeFromTop(charCurveHeight);
    band2CtrlArea.removeFromTop(charCurveHeight);
    band3CtrlArea.removeFromTop(charCurveHeight);
    band4CtrlArea.removeFromTop(charCurveHeight);

    //drive & level
    auto driveLevelHeight = int(totalBandAreaHeight * 0.4);
    auto driveWidthRatio = 0.6; 
    auto levelWidthRatio = 1.0 - driveWidthRatio;

    auto driveArea1 = band1CtrlArea.removeFromTop(driveLevelHeight);
    band1Drive.setBounds(driveArea1.removeFromLeft(bandWidth * driveWidthRatio).reduced(padding / 2));
    band1Level.setBounds(driveArea1.reduced(padding / 2)); // Takes remaining width

    auto driveArea2 = band2CtrlArea.removeFromTop(driveLevelHeight);
    band2Drive.setBounds(driveArea2.removeFromLeft(bandWidth * driveWidthRatio).reduced(padding / 2));
    band2Level.setBounds(driveArea2.reduced(padding / 2));

    auto driveArea3 = band3CtrlArea.removeFromTop(driveLevelHeight);
    band3Drive.setBounds(driveArea3.removeFromLeft(bandWidth * driveWidthRatio).reduced(padding / 2));
    band3Level.setBounds(driveArea3.reduced(padding / 2));

    auto driveArea4 = band4CtrlArea.removeFromTop(driveLevelHeight);
    band4Drive.setBounds(driveArea4.removeFromLeft(bandWidth * driveWidthRatio).reduced(padding / 2));
    band4Level.setBounds(driveArea4.reduced(padding / 2));

    //distortion types
    auto comboBoxHeight = int(totalBandAreaHeight * 0.1);
    band1Selector.setBounds(band1CtrlArea.removeFromTop(comboBoxHeight).reduced(padding / 2));
    band2Selector.setBounds(band2CtrlArea.removeFromTop(comboBoxHeight).reduced(padding / 2));
    band3Selector.setBounds(band3CtrlArea.removeFromTop(comboBoxHeight).reduced(padding / 2));
    band4Selector.setBounds(band4CtrlArea.removeFromTop(comboBoxHeight).reduced(padding / 2));


    //mute/Solo buttons
    auto muteSoloArea1 = band1CtrlArea;
    auto muteSoloWidth = muteSoloArea1.getWidth() / 2;
    band1MuteButton.setBounds(muteSoloArea1.removeFromLeft(muteSoloWidth).reduced(padding));
    band1SoloButton.setBounds(muteSoloArea1.reduced(padding));

    auto muteSoloArea2 = band2CtrlArea;
    band2MuteButton.setBounds(muteSoloArea2.removeFromLeft(muteSoloWidth).reduced(padding));
    band2SoloButton.setBounds(muteSoloArea2.reduced(padding));

    auto muteSoloArea3 = band3CtrlArea;
    band3MuteButton.setBounds(muteSoloArea3.removeFromLeft(muteSoloWidth).reduced(padding));
    band3SoloButton.setBounds(muteSoloArea3.reduced(padding));

    auto muteSoloArea4 = band4CtrlArea;
    band4MuteButton.setBounds(muteSoloArea4.removeFromLeft(muteSoloWidth).reduced(padding));
    band4SoloButton.setBounds(muteSoloArea4.reduced(padding));

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

void MBDistortionAudioProcessorEditor::drawCharacteristicCurve(juce::Graphics& g, juce::Rectangle<int> bounds,
    DistortionTypes type, float drive, float level) {
    
    //style stuff
    auto curveBgColour = juce::Colour(0xff323940);
    auto axisColour = juce::Colour(0xffe6d9c5).withAlpha(0.6f);
    auto curveColour = juce::Colour(0xff82585b);

    //set background
    g.setColour(curveBgColour);
    g.fillRect(bounds);
    
    //convert to gain
    float driveGain = pow(10, drive / 20.0f);
    float levelGain = pow(10, level / 20.0f);

    //temp processors
    DistortionProcessor tempDistortion;
    tempDistortion.setDistortionType(type);

    const float inputMin = -1.0f;
    const float inputMax = 1.0f;
    const float displayOutputMin = -1.2f;
    const float displayOutputMax = 1.2f;

    //x axis
    float yZero = juce::jmap(0.0f, displayOutputMin, displayOutputMax, (float)bounds.getBottom(), (float)bounds.getY());
    yZero = juce::jlimit((float)bounds.getY(), (float)bounds.getBottom(), yZero); 
    g.setColour(axisColour);
    g.drawLine((float)bounds.getX(), yZero, (float)bounds.getRight(), yZero);

    //y axis
    float xZero = juce::jmap(0.0f, inputMin, inputMax, (float)bounds.getX(), (float)bounds.getRight());
    xZero = juce::jlimit((float)bounds.getX(), (float)bounds.getRight(), xZero);
    g.drawLine(xZero, (float)bounds.getY(), xZero, (float)bounds.getBottom(), 0.8f); 

    juce::Path path;
    const int numPoints = bounds.getWidth(); 
    bool firstPoint = true;

    for (int i = 0; i < numPoints; ++i)
    {
        float currentInput = juce::jmap((float)i, 0.0f, (float)(numPoints - 1), inputMin, inputMax);

        float drivenInput = currentInput * driveGain;

        float processedSample = tempDistortion.processSample(drivenInput);

        processedSample *= levelGain;

        float x = juce::jmap(currentInput, inputMin, inputMax,
            (float)bounds.getX(), (float)bounds.getRight());
        float y = juce::jmap(processedSample, displayOutputMin, displayOutputMax,
            (float)bounds.getBottom(), (float)bounds.getY());

        y = juce::jlimit((float)bounds.getY(), (float)bounds.getBottom(), y);

        if (firstPoint) {
            path.startNewSubPath(x, y);
            firstPoint = false;
        }
        else {
            path.lineTo(x, y);
        }
    }

    g.setColour(curveColour);
    g.strokePath(path, juce::PathStrokeType(1.8f)); 

}

CustomLookAndFeel::CustomLookAndFeel() {
    //things for sliders
    //other bits
    //using https://pigmentsapp.com/generator to get argb values

    auto sliderThumbColour = juce::Colour(0xffb0928b);
    auto sliderTrackColour = juce::Colour(0xffc77373);
    setColour(juce::Slider::thumbColourId, sliderThumbColour);
    setColour(juce::Slider::trackColourId, sliderTrackColour);

    auto rotarySliderFillColour = juce::Colour(0xffc77373);
    setColour(juce::Slider::rotarySliderFillColourId, rotarySliderFillColour);

    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff323940));
    setColour(juce::ComboBox::arrowColourId, juce::Colour(0xff82585b));

}