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
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
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
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName(int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    const int numInputChannels = 1;
    const int delayBufferSize = 16384;
    mDelayBuffer.setSize(numInputChannels, delayBufferSize);
    // clear delaybuffer to prevent feedback / NaNs
    for (auto i = 0; i < numInputChannels; ++i)
        mDelayBuffer.clear(i, 0, delayBufferSize);
}

void NewProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void NewProjectAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    mInputBuffer.setSize(1, buffer.getNumSamples());
    mOutputBuffer.setSize(getTotalNumInputChannels(), buffer.getNumSamples());
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = totalNumInputChannels - 1; channel >= 0; --channel)
    {
        //auto* channelData = buffer.getWritePointer (channel);
        // ..do something to the data...
        const int bufferLength = buffer.getNumSamples();
        const int numInputChannels = getTotalNumInputChannels();
        const int delayBufferLength = mDelayBuffer.getNumSamples();
        mInputGain = 1.0 / totalNumInputChannels;
        mInputBuffer.addFrom(0, 0, buffer, channel, 0, bufferLength, mInputGain);

        //copy the data from main buffer to delay buffer
        if (channel == 0)
        {
            for (int y = 0; y < bufferLength; y++)
            {
                /*
                mInputBuffer.addSample(0, y, mFeedbackTaps);
                */

                //calculate base address factors
                gainBaseAddr = (decayTime << 5) | (program << 8);
                preDelay_high = preDelay >> 3;
                preDelay_low = preDelay << 5;
                preDelay_low = preDelay_low >> 5;
                delayBaseAddr = (preDelay_low << 6) | (program << 9) | (preDelay_high << 12);

                //calculate write tap (=test tap)
                //calculate address row
                result = nROW;
                bit6 = result << 1;
                bit6 = bit6 >> 7;
                MSB = result;
                MSB = MSB >> 7;
                delayCarryOut = result >> 8;
                rowDelay = result << 2;
                rowDelay = (rowDelay >> 1) | bit6 | (MSB << 7);
                //calculate address column
                result = nCOLUMN + delayCarryOut;
                colDelay = result << 2;
                colDelay = colDelay >> 2;
                //store address
                mWritePosition = static_cast<int>((rowDelay) + (colDelay * 256));
                //write sample
                mDelayBuffer.copyFrom(channel, mWritePosition, mInputBuffer, channel, y, 1);

                //calculate feedback taps
                dly_mod_addr = delayModBaseAddr + 7;
                dly_addr = delayBaseAddr + 16;
                gainModContAddress = gainModContBaseAddr + 8;
                gainAddress = gainBaseAddr + 8;
                for (int d = 0; d < 15; d++)
                {

                    result = U79[dly_mod_addr + d] + nROW;
                    bit6 = result << 1;
                    bit6 = bit6 >> 7;
                    MSB = result;
                    MSB = MSB >> 7;
                    delayCarryOut = result >> 8;
                    rowDelay = result << 2;
                    rowDelay = (rowDelay >> 1) | bit6 | (MSB << 7);

                    result = U69[dly_addr + d * 2] + nCOLUMN + delayCarryOut;
                    colDelay = result << 2;
                    colDelay = colDelay >> 2;

                    delayTaps[1 + d] = (rowDelay) + (colDelay * 256);

                    gainModContOut = U76[gainModContAddress + d];
                    nGainModEnable = gainModContOut >> 3;
                    gainModContOut = gainModContOut << 5;
                    gainModContOut = gainModContOut >> 5;

                    gainModAddress = gainModContOut | gainModBaseAddr;
                    gainModOut = U77[gainModAddress];

                    gainOut = U78[gainAddress + d] << 1;

                    if (gainModOut < gainOut && nGainModEnable == 0)
                    {
                        gainCeiling[1 + d] = gainModOut;
                    }
                    else
                    {
                        gainCeiling[1 + d] = gainOut;
                    }

                    nGSN = U78[gainAddress + d] >> 7;
                    signMod[1 + d] = nGSN;

                    mReadPosition = static_cast<int>(delayTaps[1 + d]);
                    if (signMod[1 + d] == 0)
                    {
                        mFeedbackGain = (gainCeiling[1 + d] / 256.0) * -1.0;
                    }
                    else
                    {
                        mFeedbackGain = (gainCeiling[1 + d] / 256.0);
                    }
                    mFeedbackTaps += mDelayBuffer.getSample(0, mReadPosition) * mFeedbackGain;
                }
                mFeedbackTaps = mFeedbackTaps / 15.0;

                //calculate output taps
                gainAddress = gainBaseAddr + 23;
                dly_mod_addr = delayBaseAddr + 45;
                dly_addr = delayBaseAddr + 46;
                //left output taps
                for (int d = 0; d < 3; d++)
                {
                    result = U69[dly_mod_addr + d * 2] + nROW;
                    bit6 = result << 1;
                    bit6 = bit6 >> 7;
                    MSB = result;
                    MSB = MSB >> 7;
                    delayCarryOut = result >> 8;
                    rowDelay = result << 2;
                    rowDelay = (rowDelay >> 1) | bit6 | (MSB << 7);

                    result = U69[dly_addr + d * 2] + nCOLUMN + delayCarryOut;
                    colDelay = result << 2;
                    colDelay = colDelay >> 2;

                    delayTaps[16 + d] = (rowDelay) + (colDelay * 256);

                    gainOut = U78[gainAddress + d] << 1;

                    gainCeiling[16 + d] = gainOut;

                    nGSN = U78[gainAddress + d] >> 7;
                    signMod[16 + d] = nGSN;

                    mReadPosition = static_cast<int>(delayTaps[16 + d]);
                    if (signMod[16 + d] == 0)
                    {
                        mOutputGain = (gainCeiling[16 + d] / 256.0) * -1.0;
                    }
                    else
                    {
                        mOutputGain = (gainCeiling[16 + d] / 256.0);
                    }
                    mOutputTaps += mDelayBuffer.getSample(0, mReadPosition) * mOutputGain;
                }
                mOutputTaps = mOutputTaps / 4.0;
                mOutputBuffer.setSample(0, y, mOutputTaps);
                //right output taps
                for (int d = 4; d < 8; d++)
                {
                    result = U69[dly_mod_addr + d * 2] + nROW;
                    bit6 = result << 1;
                    bit6 = bit6 >> 7;
                    MSB = result;
                    MSB = MSB >> 7;
                    delayCarryOut = result >> 8;
                    rowDelay = result << 2;
                    rowDelay = (rowDelay >> 1) | bit6 | (MSB << 7);

                    result = U69[dly_addr + d * 2] + nCOLUMN + delayCarryOut;
                    colDelay = result << 2;
                    colDelay = colDelay >> 2;

                    delayTaps[16 + d] = (rowDelay) + (colDelay * 256);

                    gainOut = U78[gainAddress + d] << 1;

                    gainCeiling[16 + d] = gainOut;

                    nGSN = U78[gainAddress + d] >> 7;
                    signMod[16 + d] = nGSN;

                    mReadPosition = static_cast<int>(delayTaps[16 + d]);
                    if (signMod[16 + d] == 0)
                    {
                        mOutputGain = (gainCeiling[16 + d] / 256.0) * -1.0;
                    }
                    else
                    {
                        mOutputGain = (gainCeiling[16 + d] / 256.0);
                    }
                    mOutputTaps += mDelayBuffer.getSample(0, mReadPosition) * mOutputGain;
                }
                mOutputTaps = mOutputTaps / 4.0;
                //const float *outputTapsData = mOutputTaps;
                mOutputBuffer.setSample(1, y, mOutputTaps);

                //mod rate counter
                modClockOut = modClockOut + 1;
                if (modClockOut == 16)
                {
                    modRateCount = rateLvl | (program << 4);
                    modClockOut = U71[modRateCount];
                }
                modCarry = (modClockOut + 1) >> 4;

                //advance write address & wraparound if < 0
                writeAddressCount = writeAddressCount - 1;
                if (writeAddressCount < 0)
                {
                    writeAddressCount = 16383;
                }
                nROW = writeAddressCount;
                nCOLUMN = writeAddressCount >> 8;
                MCCK = modCarry;
                if (MCCK == 1)
                {
                    modCount = modCount + 1;
                    if (modCount > 8191)
                    {
                        modCount = 0;
                    }

                    gainModContBaseAddr = (modCount >> 6) << 5;
                    gainModBaseAddr = modCount << 7;
                    gainModBaseAddr = gainModBaseAddr >> 4;
                    delayModCount = modCount >> 6;
                    delayModBaseAddr = delayModCount << 5;
                }
            }
            //add output samples to buffer
            buffer.copyFrom(0, 0, mOutputBuffer, 0, 0, bufferLength);
            buffer.copyFrom(1, 0, mOutputBuffer, 1, 0, bufferLength);
            //clear input buffer
            mInputBuffer.clear(0, 0, bufferLength);
        }
    }
}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor(*this);
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NewProjectAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}
