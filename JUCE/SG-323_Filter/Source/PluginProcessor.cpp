/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
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

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

//==============================================================================
const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    lastSampleRate = sampleRate;
    halfSampleRate = sampleRate * 0.5f;
    s1b0 = 1.0f;
    s1b1 = 1.224283695220947265625f;
    s1b2 = 1.0f;
    s1a0 = 1.0f;
    s1a1 = 0.4837161600589752197265625f;
    s1a2 = 0.717677175998687744140625f;
    s2b0 = 1.0f;
    s2b1 = 1.828480243682861328125f;
    s2b2 = 1.0f;
    s2a0 = 1.0f;
    s2a1 = -0.2016279697418212890625f;
    s2a2 = 0.19512404501438140869140625f;
    s3b0 = 1.0f;
    s3b1 = 0.982987344264984130859375f;
    s3b2 = 1.0f;
    s3a0 = 1.0f;
    s3a1 = 0.7548847198486328125f;
    s3a2 = 0.942220151424407958984375f;
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    inputHighPass.prepare(spec);
    inputHighPass.reset();
    inputLowPass.prepare(spec);
    inputLowPass.reset();
    preEmphasis.prepare(spec);
    preEmphasis.reset();
    deEmphasis.prepare(spec);
    deEmphasis.reset();
    feedBackHighPass.prepare(spec);
    feedBackHighPass.reset();
    feedBackLowPass.prepare(spec);
    feedBackLowPass.reset();
    feedBackDip.prepare(spec);
    feedBackDip.reset();
    antiAliasFirstStage.prepare(spec);
    antiAliasFirstStage.reset();
    antiAliasSecondStage.prepare(spec);
    antiAliasSecondStage.reset();
    antiAliasThirdStage.prepare(spec);
    antiAliasThirdStage.reset();
    gainModule.prepare(spec);
}

void NewProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void NewProjectAudioProcessor::updateFilter()
{
    *inputHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(lastSampleRate, 14.0f);
    *inputLowPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(lastSampleRate, halfSampleRate);
    *preEmphasis.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(lastSampleRate, 4000.0f, 0.5f, 4.0f);
    *deEmphasis.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(lastSampleRate, 4000.0f, 0.5f, 0.25f);
    *feedBackHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(lastSampleRate, 22.0f);
    *feedBackLowPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(lastSampleRate, 18000.0f);
    *feedBackDip.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(lastSampleRate, 9000.0f, 0.5f, 0.85f);
    *antiAliasFirstStage.state = juce::dsp::IIR::Coefficients<float>::Coefficients(s1b0,s1b1,s1b2,s1a0,s1a1,s1a2);
    *antiAliasSecondStage.state = juce::dsp::IIR::Coefficients<float>::Coefficients(s2b0, s2b1, s2b2, s2a0, s2a1, s2a2);
    *antiAliasThirdStage.state = juce::dsp::IIR::Coefficients<float>::Coefficients(s3b0, s3b1, s3b2, s3a0, s3a1, s3a2);
}

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    juce::dsp::AudioBlock <float> block (buffer);
    //DSP: inputSum > preEmphasis > addFeedback > variableHPF > variableLPF > antiAlias > roundBits > Delay+Gain > sumFeedback, sumLeftOutput, sumRightOutput > deEmphasis > antiAlias
    updateFilter();
    preEmphasis.process(juce::dsp::ProcessContextReplacing <float>(block));
    inputHighPass.process(juce::dsp::ProcessContextReplacing <float>(block));
    //inputLowPass.process(juce::dsp::ProcessContextReplacing <float>(block));
    deEmphasis.process(juce::dsp::ProcessContextReplacing <float> (block));
    gainModule.setGainLinear(1.16304862499237060546875f);
    gainModule.process(juce::dsp::ProcessContextReplacing <float>(block));
    antiAliasFirstStage.process(juce::dsp::ProcessContextReplacing <float>(block));
    gainModule.setGainLinear(3.4951908588409423828125f);
    gainModule.process(juce::dsp::ProcessContextReplacing <float>(block));
    antiAliasSecondStage.process(juce::dsp::ProcessContextReplacing <float>(block));
    gainModule.setGainLinear(0.0372033305466175079345703125f);
    gainModule.process(juce::dsp::ProcessContextReplacing <float>(block));
    antiAliasThirdStage.process(juce::dsp::ProcessContextReplacing <float>(block));
    /*feedBackHighPass.process(juce::dsp::ProcessContextReplacing <float>(block));
    feedBackLowPass.process(juce::dsp::ProcessContextReplacing <float>(block));
    feedBackDip.process(juce::dsp::ProcessContextReplacing <float>(block));*/
}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}
