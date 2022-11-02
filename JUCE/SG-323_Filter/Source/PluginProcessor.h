/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class NewProjectAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    NewProjectAudioProcessor();
    ~NewProjectAudioProcessor() override;

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

    void updateFilter();

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewProjectAudioProcessor)
    float lastSampleRate;
    float halfSampleRate;
    float s1a0;
    float s1a1;
    float s1a2;
    float s1b0;
    float s1b1;
    float s1b2;
    float s2a0;
    float s2a1;
    float s2a2;
    float s2b0;
    float s2b1;
    float s2b2;
    float s3a0;
    float s3a1;
    float s3a2;
    float s3b0;
    float s3b1;
    float s3b2;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> inputHighPass;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> inputLowPass;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> preEmphasis;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> deEmphasis;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> feedBackDip;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> feedBackHighPass;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> feedBackLowPass;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> antiAliasFirstStage;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> antiAliasSecondStage;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> antiAliasThirdStage;
    juce::dsp::Gain<float> gainModule;
};
