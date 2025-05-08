/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FilterClasses.h"
#include "DistortionProcessor.h"

//==============================================================================
/**
*/
class MBDistortionAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MBDistortionAudioProcessor();
    ~MBDistortionAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void setBandDistortionType(int bandIndex, DistortionTypes type);

    //==============================================================================
    
    //APVTS
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState parameters{ *this, nullptr, "Parameters", createParameterLayout() };

private:
    //==============================================================================
    
    //define filters for bands
    //4 bands needs 6 filters
    //1 for lowband, 2 for 1st band, 2 for 2nd band, 1 for highband
    //low band
    std::vector<LinkwitzRileyLowPass> mLowBandLP;
    //lowmid band
    std::vector<LinkwitzRileyHighPass> mLowMidBandHP;
    std::vector<LinkwitzRileyLowPass> mLowMidBandLP;
    //highmid band
    std::vector<LinkwitzRileyHighPass> mHighMidBandHP;
    std::vector<LinkwitzRileyLowPass> mHighMidBandLP;
    //high band
    std::vector<LinkwitzRileyHighPass> mHighBandHP;

    //store temp crossover freqs
    float crossoverFreq1 = 200.0f;
    float crossoverFreq2 = 1000.0f;
    float crossoverFreq3 = 5000.0f;

    //distortion processor
    DistortionProcessor lowBandDistortion;
    DistortionProcessor lowMidBandDistortion;
    DistortionProcessor highMidBandDistortion;
    DistortionProcessor highBandDistortion;

    //spectrum visualisation
    //juce::dsp::FFT mFFT;
    //juce::dsp::WindowingFunction<float> mWindow;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MBDistortionAudioProcessor)
        
};
