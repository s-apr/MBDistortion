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

//lecture 12 ring buffer
class RingBuffer
{
public:
    void resize(int newSize) { mBuffer.assign(newSize, 0.0f); }
    bool hasNewData() const { return mReadPos != mWritePos; }
    void write(float v) { mBuffer[mWritePos] = v; mWritePos = (mWritePos + 1) % size(); }
    float read() { auto v = mBuffer[mReadPos]; mReadPos = (mReadPos + 1) % size(); return v; }
    int size() const { return (int)mBuffer.size(); }

private:
    std::atomic<int> mReadPos{ 0 }, mWritePos{ 0 };
    std::vector<float> mBuffer;
};

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
    void updateOversamplefactor();
    const double getEffectiveSampleRate();

    //==============================================================================
    
    //APVTS
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState parameters{ *this, nullptr, "Parameters", createParameterLayout() };

    //osc ringbuffer
    RingBuffer oscBuffer;

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

    //oversampling (to avoid aliasing) global oversample
    std::vector<std::unique_ptr<juce::dsp::Oversampling<float>>> mOversample;
    float mHostSampleRate = 44100;
    int mCurrentOversamplingFactor = 1; //1 = "Off"
    bool mOversamplingUpdate = true;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MBDistortionAudioProcessor)   
};
