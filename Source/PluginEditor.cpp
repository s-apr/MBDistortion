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
    band1DriveLabel.setText("Drive", juce::dontSendNotification);
    band1DriveLabel.attachToComponent(&band1Drive, false);
    band1DriveLabel.setJustificationType(juce::Justification::centred);

    addSliderRotary(band2Drive);
    band2DriveLabel.setText("Drive", juce::dontSendNotification);
    band2DriveLabel.attachToComponent(&band2Drive, false);
    band2DriveLabel.setJustificationType(juce::Justification::centred);

    addSliderRotary(band3Drive);
    band3DriveLabel.setText("Drive", juce::dontSendNotification);
    band3DriveLabel.attachToComponent(&band3Drive, false);
    band3DriveLabel.setJustificationType(juce::Justification::centred);
   
    addSliderRotary(band4Drive);
    band4DriveLabel.setText("Drive", juce::dontSendNotification);
    band4DriveLabel.attachToComponent(&band4Drive, false);
    band4DriveLabel.setJustificationType(juce::Justification::centred);

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
    oversampleSelector.setJustificationType(juce::Justification::centred);
    oversampleSelector.setText("Oversampling", juce::dontSendNotification);

    addAndMakeVisible(oscilloscope);

    //global controls
    addSliderRotary(inputGainSlider);
    inputGainLabel.setText("Input Gain", juce::dontSendNotification);
    inputGainLabel.attachToComponent(&inputGainSlider, false);
    inputGainLabel.setJustificationType(juce::Justification::centred);

    addSliderRotary(outputGainSlider);
    outputGainLabel.setText("Output Gain", juce::dontSendNotification);
    outputGainLabel.attachToComponent(&outputGainSlider, false);
    outputGainLabel.setJustificationType(juce::Justification::centred);

    addSliderRotary(masterMixSlider);
    masterMixLabel.setText("Master Mix", juce::dontSendNotification);
    masterMixLabel.attachToComponent(&masterMixSlider, false);
    masterMixLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("Bypass");

    //slider crossovver
    addSliderHorizontal(crossover1Slider);
    crossover1Slider.setTextValueSuffix(" Hz");
    crossover1Label.setText("Crossover 1", juce::dontSendNotification);
    crossover1Label.attachToComponent(&crossover1Slider, false);
    crossover1Label.setJustificationType(juce::Justification::centred);

    addSliderHorizontal(crossover2Slider);
    crossover2Slider.setTextValueSuffix(" Hz");
    crossover2Label.setText("Crossover 2", juce::dontSendNotification);
    crossover2Label.attachToComponent(&crossover2Slider, false);
    crossover2Label.setJustificationType(juce::Justification::centred);


    addSliderHorizontal(crossover3Slider);
    crossover3Slider.setTextValueSuffix(" Hz");
    crossover3Label.setText("Crossover 3", juce::dontSendNotification);
    crossover3Label.attachToComponent(&crossover3Slider, false);
    crossover3Label.setJustificationType(juce::Justification::centred);


    //band level
    addSliderVertical(band1Level);
    band1LevelLabel.setText("Level", juce::dontSendNotification);
    band1LevelLabel.attachToComponent(&band1Level, false);
    band1LevelLabel.setJustificationType(juce::Justification::centred);

    addSliderVertical(band2Level);
    band2LevelLabel.setText("Level", juce::dontSendNotification);
    band2LevelLabel.attachToComponent(&band2Level, false);
    band2LevelLabel.setJustificationType(juce::Justification::centred);

    addSliderVertical(band3Level);
    band3LevelLabel.setText("Level", juce::dontSendNotification);
    band3LevelLabel.attachToComponent(&band3Level, false);
    band3LevelLabel.setJustificationType(juce::Justification::centred);

    addSliderVertical(band4Level);
    band4LevelLabel.setText("Level", juce::dontSendNotification);
    band4LevelLabel.attachToComponent(&band4Level, false);
    band4LevelLabel.setJustificationType(juce::Justification::centred);

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

    //had to put these down here
    //wouldn't set text otherwise
    //something to do with how combo boxes are populated
    //idk
    band1Selector.setText("Distortion Type", juce::dontSendNotification);
    band2Selector.setText("Distortion Type", juce::dontSendNotification);
    band3Selector.setText("Distortion Type", juce::dontSendNotification);
    band4Selector.setText("Distortion Type", juce::dontSendNotification);

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
    int bandLabelHeight = 20;

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

    auto crossover1Area = crossoverArea.removeFromLeft(crossoverSliderWidth);
    crossover1Label.setBounds(crossover1Area.removeFromTop(bandLabelHeight).reduced(padding / 2));
    crossover1Slider.setBounds(crossover1Area.reduced(padding / 2));

    auto crossover2Area = crossoverArea.removeFromLeft(crossoverSliderWidth);
    crossover2Label.setBounds(crossover2Area.removeFromTop(bandLabelHeight).reduced(padding / 2));
    crossover2Slider.setBounds(crossover2Area.reduced(padding / 2));

    auto crossover3Area = crossoverArea;
    crossover3Label.setBounds(crossover3Area.removeFromTop(bandLabelHeight).reduced(padding / 2));
    crossover3Slider.setBounds(crossover3Area.reduced(padding / 2));

    //gap
    area.removeFromTop(padding / 2);

    //global controls
    auto globalHeight = int(availableHeight * 0.15);
    auto globalArea = area.removeFromTop(globalHeight);
    auto globalCtrlWidth = globalArea.getWidth() / 5; //5 global controls

    auto inputGainArea = globalArea.removeFromLeft(globalCtrlWidth);
    inputGainLabel.setBounds(inputGainArea.removeFromTop(bandLabelHeight).reduced(padding / 2));
    inputGainSlider.setBounds(inputGainArea.reduced(padding / 2));

    auto masterMixArea = globalArea.removeFromLeft(globalCtrlWidth);
    masterMixLabel.setBounds(masterMixArea.removeFromTop(bandLabelHeight).reduced(padding / 2)); 
    masterMixSlider.setBounds(masterMixArea.reduced(padding / 2));

    auto outputGainArea = globalArea.removeFromLeft(globalCtrlWidth);
    outputGainLabel.setBounds(outputGainArea.removeFromTop(bandLabelHeight).reduced(padding / 2));
    outputGainSlider.setBounds(outputGainArea.reduced(padding / 2));

    auto bypassArea = globalArea.removeFromLeft(globalCtrlWidth);
    bypassLabel.setBounds(bypassArea.removeFromTop(bandLabelHeight).reduced(padding / 2));
    bypassButton.setBounds(bypassArea.reduced(padding / 2));

    auto oversampleArea = globalArea;
    oversampleSelector.setBounds(oversampleArea.reduced(padding / 2));

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

    //band1
    auto band1DriveLevelArea = band1CtrlArea.removeFromTop(driveLevelHeight);
    auto band1DriveArea = band1DriveLevelArea.removeFromLeft(bandWidth * driveWidthRatio); 
    auto band1LevelArea = band1DriveLevelArea; 
    band1DriveLabel.setBounds(band1DriveArea.removeFromTop(bandLabelHeight).reduced(padding / 2));
    band1Drive.setBounds(band1DriveArea.reduced(padding / 2)); 
    band1LevelLabel.setBounds(band1LevelArea.removeFromTop(bandLabelHeight).reduced(padding / 2));
    band1Level.setBounds(band1LevelArea.reduced(padding / 2));

    auto band2DriveLevelArea = band2CtrlArea.removeFromTop(driveLevelHeight);
    auto band2DriveArea = band2DriveLevelArea.removeFromLeft(bandWidth * driveWidthRatio);
    auto band2LevelArea = band2DriveLevelArea;
    band2DriveLabel.setBounds(band2DriveArea.removeFromTop(bandLabelHeight).reduced(padding / 2));
    band2Drive.setBounds(band2DriveArea.reduced(padding / 2));
    band2LevelLabel.setBounds(band2LevelArea.removeFromTop(bandLabelHeight).reduced(padding / 2));
    band2Level.setBounds(band2LevelArea.reduced(padding / 2));

    auto band3DriveLevelArea = band3CtrlArea.removeFromTop(driveLevelHeight);
    auto band3DriveArea = band3DriveLevelArea.removeFromLeft(bandWidth * driveWidthRatio);
    auto band3LevelArea = band3DriveLevelArea;
    band3DriveLabel.setBounds(band3DriveArea.removeFromTop(bandLabelHeight).reduced(padding / 2));
    band3Drive.setBounds(band3DriveArea.reduced(padding / 2));
    band3LevelLabel.setBounds(band3LevelArea.removeFromTop(bandLabelHeight).reduced(padding / 2));
    band3Level.setBounds(band3LevelArea.reduced(padding / 2));

    auto band4DriveLevelArea = band4CtrlArea.removeFromTop(driveLevelHeight);
    auto band4DriveArea = band4DriveLevelArea.removeFromLeft(bandWidth * driveWidthRatio);
    auto band4LevelArea = band4DriveLevelArea;
    band4DriveLabel.setBounds(band4DriveArea.removeFromTop(bandLabelHeight).reduced(padding / 2));
    band4Drive.setBounds(band4DriveArea.reduced(padding / 2));
    band4LevelLabel.setBounds(band4LevelArea.removeFromTop(bandLabelHeight).reduced(padding / 2));
    band4Level.setBounds(band4LevelArea.reduced(padding / 2));

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