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
        std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID("bypass", 1), "Bypass", false),

        //filter crossover controls
        std::make_unique<juce::AudioParameterFloat>( 
            juce::ParameterID("crossoverFreq1", 1), "Crossover 1 Frequency",
            juce::NormalisableRange<float>(100.0f, 500.0f, 1.0f, 0.25f),
            200.0f, "Hz"),
        std::make_unique<juce::AudioParameterFloat>( 
            juce::ParameterID("crossoverFreq2", 1), "Crossover 2 Frequency",
            juce::NormalisableRange<float>(400.0f, 2500.0f, 1.0f, 0.25f),
            1000.0f, "Hz"),
        std::make_unique<juce::AudioParameterFloat>( 
            juce::ParameterID("crossoverFreq3", 1), "Crossover 3 Frequency",
            juce::NormalisableRange<float>(2000.0f, 10000.0f, 1.0f, 0.25f),
            5000.0f, "Hz"),

        //oversampling options
        std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("oversamplingFactor", 1),
            "Oversampling",
            juce::StringArray{"Off", "2x", "4x", "8x"},
            0), //default "Off"

        //band levels
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("band1level", 1), "Low Band Level",
            juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f, 1.0),
            0.0f, "dB"),
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("band2level", 1), "Low Mid Band Level",
            juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f, 1.0),
            0.0f, "dB"),
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("band3level", 1), "High Mid Band Level",
            juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f, 1.0),
            0.0f, "dB"),
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("band4level", 1), "High Band Level",
            juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f, 1.0),
            0.0f, "dB"),

        std::make_unique<juce::AudioParameterBool>("band1solo", "Band 1 Solo", false),
        std::make_unique<juce::AudioParameterBool>("band1mute", "Band 1 Mute", false),
        
        std::make_unique<juce::AudioParameterBool>("band2solo", "Band 2 Solo", false),
        std::make_unique<juce::AudioParameterBool>("band2mute", "Band 2 Mute", false),

        std::make_unique<juce::AudioParameterBool>("band3solo", "Band 3 Solo", false),
        std::make_unique<juce::AudioParameterBool>("band3mute", "Band 3 Mute", false),

        std::make_unique<juce::AudioParameterBool>("band4solo", "Band 4 Solo", false),
        std::make_unique<juce::AudioParameterBool>("band4mute", "Band 4 Mute", false),

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

    //inital crossoverfreqs
    float targetFreq1 = *parameters.getRawParameterValue("crossoverFreq1");
    float targetFreq2 = *parameters.getRawParameterValue("crossoverFreq2");
    float targetFreq3 = *parameters.getRawParameterValue("crossoverFreq3");

    //enforce min separation
    //no lower than 20hz
    targetFreq1 = std::max(20.0f, targetFreq1);
    targetFreq2 = std::max(targetFreq1 + minCrossoverFreq, targetFreq2);
    targetFreq3 = std::max(targetFreq2 + minCrossoverFreq, targetFreq3);
    //ensure below nyquist
    targetFreq3 = std::min(targetFreq3, float(sampleRate / 2.0f * 0.95));
    targetFreq2 = std::min(targetFreq2, targetFreq3 - minCrossoverFreq);
    targetFreq1 = std::min(targetFreq1, targetFreq2 - minCrossoverFreq);

    lastCrossoverFreq1 = targetFreq1;
    lastCrossoverFreq2 = targetFreq2;
    lastCrossoverFreq3 = targetFreq3;

    //effective sample rate
    double effectiveSampleRate = getEffectiveSampleRate();

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

    for (int channel = 0; channel < numChannels; ++channel) {
  
        mLowBandLP[channel].setSampleRate(effectiveSampleRate);
        mLowMidBandHP[channel].setSampleRate(effectiveSampleRate);
        mLowMidBandLP[channel].setSampleRate(effectiveSampleRate);
        mHighMidBandHP[channel].setSampleRate(effectiveSampleRate);
        mHighMidBandLP[channel].setSampleRate(effectiveSampleRate);
        mHighBandHP[channel].setSampleRate(effectiveSampleRate);

        mLowBandLP[channel].setCutoff(lastCrossoverFreq1);
        mLowMidBandHP[channel].setCutoff(lastCrossoverFreq1);

        mLowMidBandLP[channel].setCutoff(lastCrossoverFreq2);
        mHighMidBandHP[channel].setCutoff(lastCrossoverFreq2);

        mHighMidBandLP[channel].setCutoff(lastCrossoverFreq3);
        mHighBandHP[channel].setCutoff(lastCrossoverFreq3);

        mLowBandLP[channel].reset();
        mLowMidBandHP[channel].reset();
        mLowMidBandLP[channel].reset();
        mHighMidBandHP[channel].reset();
        mHighMidBandLP[channel].reset();
        mHighBandHP[channel].reset();
    }

    //osc vis
    //keep 100ms for audio
    oscBuffer.resize(getSampleRate() * 0.1);

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

    bool bypassOn = (*parameters.getRawParameterValue("bypass") > 0.5f);
    
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

    float targetFreq1 = *parameters.getRawParameterValue("crossoverFreq1");
    float targetFreq2 = *parameters.getRawParameterValue("crossoverFreq2");
    float targetFreq3 = *parameters.getRawParameterValue("crossoverFreq3");
    double effectiveSampleRate = getEffectiveSampleRate();

    //enforce order & nyquist
    targetFreq1 = std::clamp(targetFreq1, 20.0f, (float)(effectiveSampleRate / 2.0 * 0.95));
    targetFreq2 = std::clamp(targetFreq2, targetFreq1 + minCrossoverFreq, (float)(effectiveSampleRate / 2.0 * 0.95));
    targetFreq3 = std::clamp(targetFreq3, targetFreq2 + minCrossoverFreq, (float)(effectiveSampleRate / 2.0 * 0.95));
    
    if (targetFreq1 != lastCrossoverFreq1 || targetFreq2 != lastCrossoverFreq2 || targetFreq3 != lastCrossoverFreq3) {
        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            mLowBandLP[channel].setCutoff(targetFreq1);
            mLowMidBandHP[channel].setCutoff(targetFreq1);

            mLowMidBandLP[channel].setCutoff(targetFreq2);
            mHighMidBandHP[channel].setCutoff(targetFreq2);

            mHighMidBandLP[channel].setCutoff(targetFreq3);
            mHighBandHP[channel].setCutoff(targetFreq3);
        }

        lastCrossoverFreq1 = targetFreq1;
        lastCrossoverFreq2 = targetFreq2;
        lastCrossoverFreq3 = targetFreq3;
    }

    float band1Drive = pow(10, *parameters.getRawParameterValue("band1drive") / 20.0f);
    float band2Drive = pow(10, *parameters.getRawParameterValue("band2drive") / 20.0f);
    float band3Drive = pow(10, *parameters.getRawParameterValue("band3drive") / 20.0f);
    float band4Drive = pow(10, *parameters.getRawParameterValue("band4drive") / 20.0f);

    //set distortion types
    lowBandDistortion.setDistortionType(static_cast<DistortionTypes>(int(*parameters.getRawParameterValue("band1type"))));
    lowMidBandDistortion.setDistortionType(static_cast<DistortionTypes>(int(*parameters.getRawParameterValue("band2type"))));
    highMidBandDistortion.setDistortionType(static_cast<DistortionTypes>(int(*parameters.getRawParameterValue("band3type"))));
    highBandDistortion.setDistortionType(static_cast<DistortionTypes>(int(*parameters.getRawParameterValue("band4type"))));

    //other params
    //inputgain, outputgain, masterMix
    float inputGain = pow(10, *parameters.getRawParameterValue("inputGain") / 20.0f);
    float outputGain = pow(10, *parameters.getRawParameterValue("outputGain") / 20.0f);
    float masterMix = *parameters.getRawParameterValue("masterMix");

    //band levels
    float band1Level = std::pow(10.0f, *parameters.getRawParameterValue("band1level") / 20.0f);
    float band2Level = std::pow(10.0f, *parameters.getRawParameterValue("band2level") / 20.0f);
    float band3Level = std::pow(10.0f, *parameters.getRawParameterValue("band3level") / 20.0f);
    float band4Level = std::pow(10.0f, *parameters.getRawParameterValue("band4level") / 20.0f);

    //SOLO&MUTE
    bool solo[4] = {
        *parameters.getRawParameterValue("band1solo") > 0.5f,
        *parameters.getRawParameterValue("band2solo") > 0.5f,
        *parameters.getRawParameterValue("band3solo") > 0.5f,
        *parameters.getRawParameterValue("band4solo") > 0.5f
    };

    bool mute[4] = {
        *parameters.getRawParameterValue("band1mute") > 0.5f,
        *parameters.getRawParameterValue("band2mute") > 0.5f,
        *parameters.getRawParameterValue("band3mute") > 0.5f,
        *parameters.getRawParameterValue("band4mute") > 0.5f
    };

    bool anySolo = solo[0] || solo[1] || solo[2] || solo[3];

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        if (mCurrentOversamplingFactor > 1) {
            auto& oversampler = *mOversample[channel];
            //'buffer' is ORIGINAL BUFFER
            auto channelBlock = juce::dsp::AudioBlock<float>(buffer).getSingleChannelBlock(channel);
            auto upscaledBlock = oversampler.processSamplesUp(channelBlock);

            //pointer to upsampled array
            float* samples = upscaledBlock.getChannelPointer(0);
            //number of samples in upsampled array
            int numOversampled = static_cast<int>(upscaledBlock.getNumSamples());

            if (bypassOn) {
                for (int i = 0; i < numOversampled; i++)
                    samples[i] *= inputGain;
                oversampler.processSamplesDown(channelBlock);
                continue;
            }

            //same sample for loop but with upsampledNumSamples
            for (int i = 0; i < numOversampled; i++)
            {
                //overall input gain - NOT DRIVE
                float input = samples[i] * inputGain;

                float low = mLowBandLP[channel].process(input);
                float lowMid = mLowMidBandLP[channel].process(mLowMidBandHP[channel].process(input));
                float highMid = mHighMidBandLP[channel].process(mHighMidBandHP[channel].process(input));
                float high = mHighBandHP[channel].process(input);

                //ACTUAL DRIVE
                low *= band1Drive;
                low = lowBandDistortion.processSample(low);
                //BAND LEVEL
                low *= band1Level;

                lowMid *= band2Drive;
                lowMid = lowMidBandDistortion.processSample(lowMid);
                lowMid *= band2Level;


                highMid *= band3Drive;
                highMid = highMidBandDistortion.processSample(highMid);
                highMid *= band3Level;


                high *= band4Drive;
                high = highBandDistortion.processSample(high);
                high *= band4Level;

                //solo, mute
                if (mute[0] || (anySolo && !solo[0])) low = 0.0f;
                if (mute[1] || (anySolo && !solo[1])) lowMid = 0.0f;
                if (mute[2] || (anySolo && !solo[2])) highMid = 0.0f;
                if (mute[3] || (anySolo && !solo[3])) high = 0.0f;

                float wet = (low + lowMid + highMid + high) * outputGain;
                samples[i] = input * (1.0f - masterMix) + wet * masterMix;

            }
            oversampler.processSamplesDown(channelBlock);
        }
        else
        {
            auto* channelData = buffer.getWritePointer(channel);

            if (bypassOn)
            {
                for (int sample = 0; sample < buffer.getNumSamples(); sample++)
                    channelData[sample] *= inputGain;
                continue;
            }

            for (int sample = 0; sample < buffer.getNumSamples(); sample++) {

                float input = channelData[sample] * inputGain;

                float low = mLowBandLP[channel].process(input);
                float lowMid = mLowMidBandLP[channel].process(mLowMidBandHP[channel].process(input));
                float highMid = mHighMidBandLP[channel].process(mHighMidBandHP[channel].process(input));
                float high = mHighBandHP[channel].process(input);

                low *= band1Drive;
                low = lowBandDistortion.processSample(low);
                lowMid *= band2Drive;
                lowMid = lowMidBandDistortion.processSample(lowMid);
                highMid *= band3Drive;
                highMid = highMidBandDistortion.processSample(highMid);
                high *= band4Drive;
                high = highBandDistortion.processSample(high);

                //solo, mute
                if (mute[0] || (anySolo && !solo[0])) low = 0.0f;
                if (mute[1] || (anySolo && !solo[1])) lowMid = 0.0f;
                if (mute[2] || (anySolo && !solo[2])) highMid = 0.0f;
                if (mute[3] || (anySolo && !solo[3])) high = 0.0f;

                float wet = (low + lowMid + highMid + high) * outputGain;
                channelData[sample] = input * (1.0f - masterMix) + wet * masterMix;

            }
        }
    }

        auto* readPtr = buffer.getReadPointer(0);
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            oscBuffer.write(readPtr[i]);
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

const double MBDistortionAudioProcessor::getEffectiveSampleRate()
{
    return mHostSampleRate * mCurrentOversamplingFactor;
}