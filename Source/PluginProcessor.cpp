/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MBDistortionAudioProcessor::MBDistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

MBDistortionAudioProcessor::~MBDistortionAudioProcessor()
{
}

//==============================================================================
//create parameter layout()
juce::AudioProcessorValueTreeState::ParameterLayout MBDistortionAudioProcessor::createParameterLayout() {
    return{
        //band drive sliders
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("band1drive", 1), "Low Band Drive",
            juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f, 0.4f),
            0.0f, "dB"),
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("band2drive", 1), "Low Mid Band Drive",
            juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f, 0.4f),
            0.0f, "dB"),
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("band3drive", 1), "High Mid Band Drive",
            juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f, 0.4f),
            0.0f, "dB"),
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("band4drive", 1), "High Band Drive",
            juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f, 0.4f),
            0.0f, "dB"),

        //band level (post)

        //combo boxes
            //move array definition into variable
            //to avoid re-definition
        std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("band1type", 1),
            "Low Band Distortion Type",
            juce::StringArray{"None", "Hard Clip", "Soft Clip", "Exponential Distortion",
            "Cubic Clip", "Arctangent", "Asymmetric", "Full Rectify", "Half Rectify"},
            0),
        std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("band2type", 1),
            "Low Mid Band Distortion Type",
            juce::StringArray{"None", "Hard Clip", "Soft Clip", "Exponential Distortion",
            "Cubic Clip", "Arctangent", "Asymmetric", "Full Rectify", "Half Rectify"},
            0),
        std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("band3type", 1),
            "High Mid Band Distortion Type",
            juce::StringArray{"None", "Hard Clip", "Soft Clip", "Exponential Distortion",
            "Cubic Clip", "Arctangent", "Asymmetric", "Full Rectify", "Half Rectify"},
            0),
        std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("band4type", 1),
            "High Band Distortion Type",
            juce::StringArray{"None", "Hard Clip", "Soft Clip", "Exponential Distortion",
            "Cubic Clip", "Arctangent", "Asymmetric", "Full Rectify", "Half Rectify"},
            0),

        //global controls
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("inputGain", 1), "Input Gain",
            juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f, 1.0),
            0.0f, "dB"),
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("outputGain", 1), "Output Gain",
            juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f, 1.0),
            0.0f, "dB"),
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("masterMix", 1), "Master Mix",
            0.0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("bypass", 1), "Bypass",
            juce::StringArray{"Off", "On"}, 0),

        //filter crossover controls
        std::make_unique<juce::AudioParameterFloat>( 
            juce::ParameterID("crossover1freq", 1), "Crossover 1 Frequency",
            juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f),
            200.0f, "Hz"),
        std::make_unique<juce::AudioParameterFloat>( 
            juce::ParameterID("crossover2freq", 1), "Crossover 2 Frequency",
            juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f),
            1000.0f, "Hz"),
        std::make_unique<juce::AudioParameterFloat>( 
            juce::ParameterID("crossover3freq", 1), "Crossover 3 Frequency",
            juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f),
            5000.0f, "Hz"),

        //oversampling options
        std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("oversamplingFactor", 1),
            "Oversampling",
            juce::StringArray{"Off", "2x", "4x", "8x"},
            0) //default "Off"
    };
}


const juce::String MBDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MBDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MBDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MBDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MBDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MBDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MBDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MBDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MBDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void MBDistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MBDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //sample rate
    mHostSampleRate = sampleRate;

    //get num channels
    int numChannels = getNumInputChannels();

    //update oversample factor first
    updateOversamplefactor();

    int oversampleStage = 0;
    if (mCurrentOversamplingFactor == 2) oversampleStage = 1;
    else if (mCurrentOversamplingFactor == 4) oversampleStage = 2;
    else if (mCurrentOversamplingFactor == 8) oversampleStage = 3;

    //setup oversamplers
    mOversample.clear();

    //create oversampler objects
    for (int channel = 0; channel < numChannels; channel++) {
        auto oversampler = std::make_unique<juce::dsp::Oversampling<float>>(
            1, oversampleStage, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);
        oversampler->initProcessing(static_cast<size_t>(samplesPerBlock));
        mOversample.push_back(std::move(oversampler));
    }

    //initalise all filter objects
    mLowBandLP.resize(numChannels);
    mLowMidBandHP.resize(numChannels);
    mLowMidBandLP.resize(numChannels);
    mHighMidBandHP.resize(numChannels);
    mHighMidBandLP.resize(numChannels);
    mHighBandHP.resize(numChannels);

    //initalise filters
    for (int channel = 0; channel < numChannels; channel++) {
        mLowBandLP[channel].setSampleRate(sampleRate);
        mLowMidBandHP[channel].setSampleRate(sampleRate);
        mLowMidBandLP[channel].setSampleRate(sampleRate);
        mHighMidBandHP[channel].setSampleRate(sampleRate);
        mHighMidBandLP[channel].setSampleRate(sampleRate);
        mHighBandHP[channel].setSampleRate(sampleRate);
    }

    //temp setting cuttoff frequencies
    for (int channel = 0; channel < numChannels; channel++)
    {
        //1st crossover
        mLowBandLP[channel].setCutoff(crossoverFreq1);
        mLowMidBandHP[channel].setCutoff(crossoverFreq1);
        //2nd crossover
        mLowMidBandLP[channel].setCutoff(crossoverFreq2);
        mHighMidBandHP[channel].setCutoff(crossoverFreq2);
        //3rd crossover
        mHighMidBandLP[channel].setCutoff(crossoverFreq3);
        mHighBandHP[channel].setCutoff(crossoverFreq3);
    }

}

void MBDistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MBDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MBDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    //oversampling
    int previousFactor = mCurrentOversamplingFactor;
    updateOversamplefactor();
    if (previousFactor != mCurrentOversamplingFactor)
        prepareToPlay(mHostSampleRate, buffer.getNumSamples());

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //clear other outputs
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float band1DriveLinear = pow(10, *parameters.getRawParameterValue("band1drive") / 20);
    float band2DriveLinear = pow(10, *parameters.getRawParameterValue("band2drive") / 20);
    float band3DriveLinear = pow(10, *parameters.getRawParameterValue("band3drive") / 20);
    float band4DriveLinear = pow(10, *parameters.getRawParameterValue("band4drive") / 20);

    //set distortion types
    lowBandDistortion.setDistortionType(static_cast<DistortionTypes>(int(*parameters.getRawParameterValue("band1type"))));
    lowMidBandDistortion.setDistortionType(static_cast<DistortionTypes>(int(*parameters.getRawParameterValue("band2type"))));
    highMidBandDistortion.setDistortionType(static_cast<DistortionTypes>(int(*parameters.getRawParameterValue("band3type"))));
    highBandDistortion.setDistortionType(static_cast<DistortionTypes>(int(*parameters.getRawParameterValue("band4type"))));

    //other params
    //inputgain, outputgain
    float inputGainLinear = pow(10, *parameters.getRawParameterValue("inputGain") / 20);
    float outputGainLinear = pow(10, *parameters.getRawParameterValue("outputGain") / 20);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        if (mCurrentOversamplingFactor > 1) {
            auto& oversampler = *mOversample[channel];
            //'buffer' is ORIGINAL BUFFER
            auto channelBlock = juce::dsp::AudioBlock<float>(buffer).getSingleChannelBlock(channel);
            auto upscaledBlock = oversampler.processSamplesUp(channelBlock);

            float* samples = upscaledBlock.getChannelPointer(0);
            int numOversampled = static_cast<int>(upscaledBlock.getNumSamples());
            
            for (int i = 0; i < numOversampled; i++)
            {
                float input = samples[i] * inputGainLinear;

                float low = mLowBandLP[channel].process(input);
                float lowMid = mLowMidBandLP[channel].process(mLowMidBandHP[channel].process(input));
                float highMid = mHighMidBandLP[channel].process(mHighMidBandHP[channel].process(input));
                float high = mHighBandHP[channel].process(input);

                low *= band1DriveLinear;
                low = lowBandDistortion.processSample(low);
                lowMid *= band2DriveLinear;
                lowMid = lowMidBandDistortion.processSample(lowMid);
                highMid *= band3DriveLinear;
                highMid = highMidBandDistortion.processSample(highMid);
                high *= band4DriveLinear;
                high = highBandDistortion.processSample(high);

                samples[i] = (low + lowMid + highMid + high) * outputGainLinear;

            }

            oversampler.processSamplesDown(channelBlock);

        }
        else
        {
            auto* channelData = buffer.getWritePointer(channel);

            for (int sample = 0; sample < buffer.getNumSamples(); sample++) {

                float input = channelData[sample] * inputGainLinear;

                float low = mLowBandLP[channel].process(input);
                float lowMid = mLowMidBandLP[channel].process(mLowMidBandHP[channel].process(input));
                float highMid = mHighMidBandLP[channel].process(mHighMidBandHP[channel].process(input));
                float high = mHighBandHP[channel].process(input);

                low *= band1DriveLinear;
                low = lowBandDistortion.processSample(low);
                lowMid *= band2DriveLinear;
                lowMid = lowMidBandDistortion.processSample(lowMid);
                highMid *= band3DriveLinear;
                highMid = highMidBandDistortion.processSample(highMid);
                high *= band4DriveLinear;
                high = highBandDistortion.processSample(high);

                channelData[sample] = (low + lowMid + highMid + high) * outputGainLinear;
            }
        }
    }
}

//==============================================================================
bool MBDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MBDistortionAudioProcessor::createEditor()
{
    return new MBDistortionAudioProcessorEditor (*this);
}

//==============================================================================
void MBDistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MBDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MBDistortionAudioProcessor();
}

//Distortion Types
void MBDistortionAudioProcessor::setBandDistortionType(int bandIndex, DistortionTypes type)
{
    switch (bandIndex)
    {
    case 0: lowBandDistortion.setDistortionType(type); break;
    case 1: lowMidBandDistortion.setDistortionType(type); break;
    case 2: highMidBandDistortion.setDistortionType(type); break;
    case 3: highBandDistortion.setDistortionType(type); break;
    default: break; // invalid index
    }
}

//oversampling
void MBDistortionAudioProcessor::updateOversamplefactor() {
    int choice = *parameters.getRawParameterValue("oversamplingFactor");
    //cast, getRawParameterValue returns float
    int choiceIndex = static_cast<int>(choice);

    switch (choiceIndex) {
        case 0: mCurrentOversamplingFactor = 1; break; //off
        case 1: mCurrentOversamplingFactor = 2; break; //x2
        case 2: mCurrentOversamplingFactor = 4; break; //x4
        case 3: mCurrentOversamplingFactor = 8; break; //x8
        default: mCurrentOversamplingFactor = 1; break; //off
    }
}

//update filters for oversampling
