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
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	), apvts(*this, nullptr, "Parameters", createParameters())
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

void NewProjectAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName(int index)
{
	return {};
}

void NewProjectAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	////legacy code block
	////initialize delay buffer
	//const int numInputChannels = 1;
	//auto delayBufferSize = 16384;
	//mDelayBuffer.setSize(numInputChannels, delayBufferSize);
	//// clear delaybuffer to prevent feedback / NaNs
	//for (auto i = 0; i < numInputChannels; ++i)
	//	mDelayBuffer.clear(i, 0, delayBufferSize);

	//reworked code block
	auto numInputChannelsTest = getTotalNumInputChannels();
	auto delayBufferTestSize = sampleRate * 0.512;
	delayBufferTest.setSize(numInputChannelsTest, static_cast<int>(delayBufferTestSize));
	for (auto i = 0; i < numInputChannelsTest; ++i)
		delayBufferTest.clear(i, 0, delayBufferTestSize);

	//set up filters
	lastSampleRate = sampleRate;
	halfSampleRate = sampleRate * 0.5f;
	juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getTotalNumOutputChannels();
	inputHighPass.prepare(spec);
	inputHighPass.reset();
	inputLowPass.prepare(spec);
	inputLowPass.reset();
	randomHighPass.prepare(spec);
	randomHighPass.reset();
	randomLowPass.prepare(spec);
	randomLowPass.reset();
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
	antiAliasFirstSection.prepare(spec);
	antiAliasFirstSection.reset();
	antiAliasSecondSection.prepare(spec);
	antiAliasSecondSection.reset();
	antiAliasThirdSection.prepare(spec);
	antiAliasThirdSection.reset();
	gainModule.prepare(spec);
	gainModule.reset();
	fractionalDelay.prepare(spec);
	fractionalDelay.reset();
	fractionalDelay.setMaximumDelayInSamples(delayBufferTestSize);
	if (sampleRate == 44100.0)
	{
		s1b0 = 1.0f;
		s1b1 = 1.4407613277435302734375f;
		s1b2 = 1.0f;
		s1a0 = 1.0f;
		s1a1 = 0.78177297115325927734375f;
		s1a2 = 0.73473322391510009765625f;
		s1gain = 1.2458820343017578125f;
		s2b0 = 1.0f;
		s2b1 = 1.88249099254608154296875f;
		s2b2 = 1.0f;
		s2a0 = 1.0f;
		s2a1 = 0.0306032598018646240234375f;
		s2a2 = 0.18833030760288238525390625f;
		s2gain = 4.228640079498291015625f;
		s3b0 = 1.0f;
		s3b1 = 1.25116384029388427734375f;
		s3b2 = 1.0f;
		s3a0 = 1.0f;
		s3a1 = 1.0544550418853759765625f;
		s3a2 = 0.9471471309661865234375f;
		s3gain = 0.037988282740116119384765625f;
		modRateCeiling = 22;
		modScale = 1.378125f;
	}
	if (sampleRate == 48000.0)
	{
		s1b0 = 1.0f;
		s1b1 = 1.224283695220947265625f;
		s1b2 = 1.0f;
		s1a0 = 1.0f;
		s1a1 = 0.4837161600589752197265625f;
		s1a2 = 0.717677175998687744140625f;
		s1gain = 1.16304862499237060546875f;
		s2b0 = 1.0f;
		s2b1 = 1.828480243682861328125f;
		s2b2 = 1.0f;
		s2a0 = 1.0f;
		s2a1 = -0.2016279697418212890625f;
		s2a2 = 0.19512404501438140869140625f;
		s2gain = 3.4951908588409423828125f;
		s3b0 = 1.0f;
		s3b1 = 0.982987344264984130859375f;
		s3b2 = 1.0f;
		s3a0 = 1.0f;
		s3a1 = 0.7548847198486328125f;
		s3a2 = 0.942220151424407958984375f;
		s3gain = 0.0372033305466175079345703125f;
		modRateCeiling = 24;
		modScale = 1.5f;
	}
	if (sampleRate == 96000.0)
	{
		s1b0 = 1.0f;
		s1b1 = -0.4685294330120086669921875f;
		s1b2 = 1.0f;
		s1a0 = 1.0f;
		s1a1 = -1.01597499847412109375f;
		s1a2 = 0.635424196720123291015625f;
		s1gain = 0.73602163791656494140625f;
		s2b0 = 1.0f;
		s2b1 = 1.1502001285552978515625f;
		s2b2 = 1.0f;
		s2a0 = 1.0f;
		s2a1 = -0.972325623035430908203125f;
		s2a2 = 0.2985363900661468505859375f;
		s2gain = 1.00631582736968994140625f;
		s3b0 = 1.0f;
		s3b1 = -0.846724808216094970703125f;
		s3b2 = 1.0f;
		s3a0 = 1.0f;
		s3a1 = -1.07159888744354248046875f;
		s3a2 = 0.901829421520233154296875f;
		s3gain = 0.0404760427772998809814453125f;
		modRateCeiling = 48;
		modScale = 3.0f;
	}
}

void NewProjectAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
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

int calculateAddress(unsigned short rowInput, unsigned short columnInput, float lastSampleRate)
{
	// calculate address row
	unsigned short result = rowInput;
	uint8_t bit6 = result << 1;
	bit6 = bit6 >> 7;
	uint8_t MSB = result;
	MSB = MSB >> 7;
	uint8_t delayCarryOut = result >> 8;
	uint8_t rowDelay = result << 2;
	rowDelay = (rowDelay >> 1) | bit6 | (MSB << 7);
	// calculate address column
	result = columnInput + delayCarryOut;
	uint8_t columnDelay = result << 2;
	columnDelay = columnDelay >> 2;
	return ((rowDelay)+(columnDelay * 256));
}

int countWriteAddress(short writeAddress)
{
	// advance write address & wraparound if < 0
	short writeAddressIncr = writeAddress - 1;
	if (writeAddressIncr < 0)
	{
		writeAddressIncr = 16383;
	}
	return writeAddressIncr;
}

float roundBits(float inputSample)
{
	short roundedSample = inputSample * 32768;
	float outputSample = roundedSample * 0.000030518;
	return (outputSample);
}

int rngsus(float randomSample) {
	int rateLevel{};
	int num1{};
	int num2{};
	int num3{};
	int num4{};

	if (randomSample >= 0.011f)
	{
		num1 = 1;
	}
	if (randomSample >= 0.0356f)
	{
		num2 = 2;
	}
	if (randomSample >= 0.0916f)
	{
		num3 = 4;
	}
	if (randomSample >= 0.317f)
	{
		num4 = 8;
	}
	rateLevel = num1 + num2 + num3 + num4;
	return(rateLevel);
}

void NewProjectAudioProcessor::updateFilter()
{
	*inputHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(lastSampleRate, 14.0f);
	*inputLowPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(lastSampleRate, halfSampleRate);
	*randomHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(lastSampleRate, 106.0f);
	*randomLowPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(lastSampleRate, 370.0f);
	*preEmphasis.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(lastSampleRate, 4000.0f, 0.5f, 4.0f);
	*deEmphasis.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(lastSampleRate, 4000.0f, 0.5f, 0.25f);
	*feedBackHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(lastSampleRate, 22.0f);
	*feedBackLowPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(lastSampleRate, 18000.0f);
	*feedBackDip.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(lastSampleRate, 9000.0f, 0.5f, 0.85f);
	*antiAliasFirstSection.state = juce::dsp::IIR::Coefficients<float>::Coefficients(s1b0, s1b1, s1b2, s1a0, s1a1, s1a2);
	*antiAliasSecondSection.state = juce::dsp::IIR::Coefficients<float>::Coefficients(s2b0, s2b1, s2b2, s2a0, s2a1, s2a2);
	*antiAliasThirdSection.state = juce::dsp::IIR::Coefficients<float>::Coefficients(s3b0, s3b1, s3b2, s3a0, s3a1, s3a2);
}

void NewProjectAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

	auto bufferSizeTest = buffer.getNumSamples();

	//read program selection from the UI
	mProgramID = *apvts.getRawParameterValue("PROGRAM");
	program = programArray[mProgramID];

	//reworked code block
	//prepare audio buffers
	monoBuffer.setSize(1, bufferSizeTest);
	mFeedbackBuffer.setSize(1, bufferSizeTest);
	mRandomBuffer.setSize(1, bufferSizeTest);
	mOutputBuffer.setSize(totalNumOutputChannels, bufferSizeTest);
	if (mSampleRateCount == 0)
	{
		mFeedbackBuffer.clear(0, 0, buffer.getNumSamples());
		mSampleRateCount = 1;
	}
	//set up dsp elements
	juce::dsp::AudioBlock <float> monoBlock(monoBuffer);
	juce::dsp::AudioBlock <float> feedbackBlock(mFeedbackBuffer);
	juce::dsp::AudioBlock <float> outputBlock(mOutputBuffer);
	juce::dsp::AudioBlock <float> randomBlock(mRandomBuffer);
	updateFilter();
	//clear buffers
	monoBuffer.clear(0, 0, bufferSizeTest);
	for (auto i = 0; i < totalNumOutputChannels; ++i)
		mOutputBuffer.clear(i, 0, bufferSizeTest);
	//sum input channels together
	monoBuffer.copyFrom(0, 0, buffer, 0, 0, bufferSizeTest);
	monoBuffer.addFrom(0, 0, buffer, 1, 0, bufferSizeTest);
	//copy & filter random Sample buffer
	mRandomBuffer.clear(0, 0, bufferSizeTest);
	mRandomBuffer.copyFrom(0, 0, monoBuffer, 0, 0, bufferSizeTest);
	randomHighPass.process(juce::dsp::ProcessContextReplacing <float>(randomBlock));
	randomLowPass.process(juce::dsp::ProcessContextReplacing <float>(randomBlock));
	//pre-process input buffer
	preEmphasis.process(juce::dsp::ProcessContextReplacing <float>(monoBlock));
	inputHighPass.process(juce::dsp::ProcessContextReplacing <float>(monoBlock));
	inputLowPass.process(juce::dsp::ProcessContextReplacing <float>(monoBlock));
	//pre-process feedback buffer
	feedBackHighPass.process(juce::dsp::ProcessContextReplacing <float>(feedbackBlock));
	feedBackLowPass.process(juce::dsp::ProcessContextReplacing <float>(feedbackBlock));
	feedBackDip.process(juce::dsp::ProcessContextReplacing <float>(feedbackBlock));
	//sum input buffer & feedback buffer together
	monoBuffer.addFrom(0, 0, mFeedbackBuffer, 0, 0, bufferSizeTest);
	auto* data = monoBuffer.getReadPointer(0);
	//clear feedback buffer
	//mFeedbackBuffer.clear(0, 0, bufferSizeTest);
	//apply anti-aliasing filter
	gainModule.setGainLinear(s1gain);
	gainModule.process(juce::dsp::ProcessContextReplacing <float>(monoBlock));
	antiAliasFirstSection.process(juce::dsp::ProcessContextReplacing <float>(monoBlock));
	gainModule.setGainLinear(s2gain);
	gainModule.process(juce::dsp::ProcessContextReplacing <float>(monoBlock));
	antiAliasSecondSection.process(juce::dsp::ProcessContextReplacing <float>(monoBlock));
	gainModule.setGainLinear(s3gain);
	gainModule.process(juce::dsp::ProcessContextReplacing <float>(monoBlock));
	antiAliasThirdSection.process(juce::dsp::ProcessContextReplacing <float>(monoBlock));
	//quantize samples to 16bit
	for (int i = 0; i < bufferSizeTest; ++i)
	{
		float trim = 0.5f;
		float sampleRounded = monoBuffer.getSample(0, i) * trim;
		monoBuffer.setSample(0, i, roundBits(sampleRounded));
	}
	//left output taps
	for (int i = 0; i < bufferSizeTest; i++)
	{
		fractionalDelay.pushSample(0, data[i]);
		// calculate base address factors
		gainBaseAddr = (decayTime << 5) | (program << 8);
		preDelay_high = preDelay >> 3;
		preDelay_low = preDelay << 5;
		preDelay_low = preDelay_low >> 5;
		delayBaseAddr = (preDelay_low << 6) | (program << 9) | (preDelay_high << 12);
		// calculate write tap (=test tap)
		int rowInput = nROW;
		int columnInput = nCOLUMN;
		mWritePosition = static_cast<int>(calculateAddress(rowInput, columnInput, lastSampleRate));
		////round sample to 16bits
		//float sampleRounded = mInputBuffer.getSample(channel, y);
		//mInputBuffer.setSample(channel, y, roundBits(sampleRounded));
		////write sample
		//mDelayBuffer.copyFrom(channel, mWritePosition, mInputBuffer, channel, y, 1);
		// calculate feedback taps
		delayModAddress = delayModBaseAddr + 7;
		delayAddress = delayBaseAddr + 16;
		gainModContAddress = gainModContBaseAddr + 8;
		gainAddress = gainBaseAddr + 8;
		for (int d = 0; d < 15; d++)
		{
			rowInput = U79[delayModAddress + d] + nROW;
			columnInput = U69[delayAddress + d * 2] + nCOLUMN;
			delayTaps[1 + d] = calculateAddress(rowInput, columnInput, lastSampleRate);
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
			mReadPosition = delayTaps[1 + d];
			if (signMod[1 + d] == 0)
			{
				mFeedbackGain = (gainCeiling[1 + d] / 256.0f) * -1.0f;
			}
			else
			{
				mFeedbackGain = (gainCeiling[1 + d] / 256.0f);
			}
			//mFeedbackTaps += mDelayBuffer.getSample(0, mReadPosition) * mFeedbackGain;
		}
		mFeedbackTaps = mFeedbackTaps / 15.0f;
		//mFeedbackBuffer.setSample(0, y, mFeedbackTaps);
		// calculate output taps
		gainAddress = gainBaseAddr + 23;
		delayModAddress = delayBaseAddr + 45;
		delayAddress = delayBaseAddr + 46;
		//left output taps
		for (int d = 0; d < 3; d++)
		{
			rowInput = U69[delayModAddress + d * 2] + nROW;
			columnInput = U69[delayAddress + d * 2] + nCOLUMN;
			delayTaps[16 + d] = calculateAddress(rowInput, columnInput, lastSampleRate);
			gainOut = U78[gainAddress + d] << 1;
			gainCeiling[16 + d] = gainOut;
			nGSN = U78[gainAddress + d] >> 7;
			signMod[16 + d] = nGSN;
			mReadPosition = delayTaps[16 + d];
			if (signMod[16 + d] == 0)
			{
				mOutputGain = (gainCeiling[16 + d] / 256.0f) * -1.0f;
			}
			else
			{
				mOutputGain = (gainCeiling[16 + d] / 256.0f);
			}
			//mOutputTaps += mDelayBuffer.getSample(0, mReadPosition) * mOutputGain;
		}
		mOutputTaps = mOutputTaps / 4.0f;
		//mOutputBuffer.setSample(0, y, mOutputTaps);
		//right output taps
		for (int d = 4; d < 8; d++)
		{
			rowInput = U69[delayModAddress + d * 2] + nROW;
			columnInput = U69[delayAddress + d * 2] + nCOLUMN;
			delayTaps[16 + d] = calculateAddress(rowInput, columnInput, lastSampleRate);
			gainOut = U78[gainAddress + d] << 1;
			gainCeiling[16 + d] = gainOut;
			nGSN = U78[gainAddress + d] >> 7;
			signMod[16 + d] = nGSN;
			mReadPosition = delayTaps[16 + d];
			if (signMod[16 + d] == 0)
			{
				mOutputGain = (gainCeiling[16 + d] / 256.0f) * -1.0f;
			}
			else
			{
				mOutputGain = (gainCeiling[16 + d] / 256.0f);
			}
			//mOutputTaps += mDelayBuffer.getSample(0, mReadPosition) * mOutputGain;
		}
		mOutputTaps = mOutputTaps / 4.0f;
		//mOutputBuffer.setSample(1, y, mOutputTaps);
		//process random sample
		float randomSample = mRandomBuffer.getSample(0, i);
		if (randomSample < 0)
		{
			randomSample = randomSample * -0.33f;
		}
		//scale randomSample by a certain amount
		randomSample = randomSample * 8.0f;
		//calculate rateLVL value
		int num1{ 0 };
		int num2{ 0 };
		int num3{ 0 };
		int num4{ 0 };
		if (randomSample >= 0.011f)
		{
			num1 = 1;
		}
		if (randomSample >= 0.0356f)
		{
			num2 = 2;
		}
		if (randomSample >= 0.0916f)
		{
			num3 = 4;
		}
		if (randomSample >= 0.317f)
		{
			num4 = 8;
		}
		rateLevel = num1 + num2 + num3 + num4;
		// mod rate counter
		modClockOut = modClockOut + 1;
		if (modClockOut == modRateCeiling)
		{
			modRateCount = rateLevel | (program << 4);
			float modClockOutScaled = U71[modRateCount] * modScale;
			modClockOut = modClockOutScaled;
		}
		modCarry = modClockOut + 1;
		if (modCarry >= modRateCeiling)
		{
			MCCK = 1;
		}
		else
		{
			MCCK = 0;
		}
		nROW = countWriteAddress(writeAddress);
		nCOLUMN = countWriteAddress(writeAddress) >> 8;
		writeAddress = countWriteAddress(writeAddress);
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
		//fractionalDelay.pushSample(0, data[i]);
		float outputDelayGainMult = 0.75f;
		float leftOutputSample{};
		float rightOutputSample{};
		float outputDelayTime{};
		float outputDelayGain{};
		for (int d = 0; d < 4; d++)
		{
			outputDelayTime = ((mProgramID * outputDelayArray[d]) + outputDelayArray[d + 8] + adjustablePreDelay) * 0.001f;
			outputDelayGain = outputGainArray[d] * outputDelayGainMult;
			leftOutputSample += fractionalDelay.popSample(0, outputDelayTime * lastSampleRate, false) * outputDelayGain;
		}
		for (int d = 4; d < 7; d++)
		{
			outputDelayTime = ((mProgramID * outputDelayArray[d]) + outputDelayArray[d + 8] + adjustablePreDelay) * 0.001f;
			outputDelayGain = outputGainArray[d] * outputDelayGainMult;
			rightOutputSample += fractionalDelay.popSample(0, outputDelayTime * lastSampleRate, false) * outputDelayGain;
		}
		outputDelayTime = ((mProgramID * outputDelayArray[7]) + outputDelayArray[7 + 8] + adjustablePreDelay) * 0.001f;
		outputDelayGain = outputGainArray[7] * outputDelayGainMult;
		rightOutputSample += fractionalDelay.popSample(0, outputDelayTime * lastSampleRate, true) * outputDelayGain;
		mOutputBuffer.setSample(0, i, leftOutputSample);
		mOutputBuffer.setSample(1, i, rightOutputSample);
	}
	//apply de-emphasis filter on output taps
	deEmphasis.process(juce::dsp::ProcessContextReplacing <float>(outputBlock));
	//write output taps to main buffer
	for (int channel = 0; channel < totalNumOutputChannels; ++channel)
	{
		buffer.copyFrom(channel, 0, mOutputBuffer, channel, 0, bufferSizeTest);
	}
	////legacy code block
	//mInputBuffer.setSize(1, buffer.getNumSamples());
	//mRandomBuffer.setSize(1, buffer.getNumSamples());
	//mOutputBuffer.setSize(totalNumOutputChannels, buffer.getNumSamples());
	//mFeedbackBuffer.setSize(1, buffer.getNumSamples());
	//if (mSampleRateCount == 0)
	//{
	//	mFeedbackBuffer.clear(0, 0, buffer.getNumSamples());
	//	mSampleRateCount = 1;
	//}
	////main loop
	//for (int channel = totalNumInputChannels - 1; channel >= 0; --channel)
	//{
	//	const int bufferLength = buffer.getNumSamples();
	//	const int numInputChannels = getTotalNumInputChannels();
	//	const int delayBufferLength = mDelayBuffer.getNumSamples();
	//	mInputGain = 1.0f / totalNumInputChannels; //Adjust input buffer gain
	//	mInputBuffer.addFrom(0, 0, buffer, channel, 0, bufferLength, mInputGain); //sum input channels together
	//	if (channel == 0) //only process delay calculation on the last channel
	//	{
	//		juce::dsp::AudioBlock <float> feedbackBlock(mFeedbackBuffer);//assign feedbackbuffer as audioblock
	//		juce::dsp::AudioBlock <float> inputBlock(mInputBuffer); //assign inputbuffer as audioblock
	//		juce::dsp::AudioBlock <float> outputBlock(mOutputBuffer); //assign outputbuffer as audioblock
	//		juce::dsp::AudioBlock <float> randomBlock(mRandomBuffer); //assign randombuffer as audioblock
	//		updateFilter();
	//		//copy & filter random Sample buffer
	//		mRandomBuffer.copyFrom(0, 0, mInputBuffer, 0, 0, bufferLength);
	//		randomHighPass.process(juce::dsp::ProcessContextReplacing <float>(randomBlock));
	//		randomLowPass.process(juce::dsp::ProcessContextReplacing <float>(randomBlock));
	//		//preEmphasis input filter
	//		preEmphasis.process(juce::dsp::ProcessContextReplacing <float>(inputBlock));
	//		inputHighPass.process(juce::dsp::ProcessContextReplacing <float>(inputBlock));
	//		inputLowPass.process(juce::dsp::ProcessContextReplacing <float>(inputBlock));
	//		//feedback filter
	//		feedBackHighPass.process(juce::dsp::ProcessContextReplacing <float>(feedbackBlock));
	//		feedBackLowPass.process(juce::dsp::ProcessContextReplacing <float>(feedbackBlock));
	//		feedBackDip.process(juce::dsp::ProcessContextReplacing <float>(feedbackBlock));
	//		//sum inputbuffer & feedbackbuffer
	//		mInputBuffer.addFrom(0, 0, mFeedbackBuffer, 0, 0, bufferLength, -2.1f);
	//		//clear feedbackbuffer
	//		mFeedbackBuffer.clear(0, 0, bufferLength);
	//		//elliptical anti aliasing filter
	//		gainModule.setGainLinear(s1gain);
	//		gainModule.process(juce::dsp::ProcessContextReplacing <float>(inputBlock));
	//		antiAliasFirstSection.process(juce::dsp::ProcessContextReplacing <float>(inputBlock));
	//		gainModule.setGainLinear(s2gain);
	//		gainModule.process(juce::dsp::ProcessContextReplacing <float>(inputBlock));
	//		antiAliasSecondSection.process(juce::dsp::ProcessContextReplacing <float>(inputBlock));
	//		gainModule.setGainLinear(s3gain);
	//		gainModule.process(juce::dsp::ProcessContextReplacing <float>(inputBlock));
	//		antiAliasThirdSection.process(juce::dsp::ProcessContextReplacing <float>(inputBlock));
	//		//iterate through all the samples in the buffer
	//		for (int y = 0; y < bufferLength; y++)
	//		{
	//			// calculate base address factors
	//			gainBaseAddr = (decayTime << 5) | (program << 8);
	//			preDelay_high = preDelay >> 3;
	//			preDelay_low = preDelay << 5;
	//			preDelay_low = preDelay_low >> 5;
	//			delayBaseAddr = (preDelay_low << 6) | (program << 9) | (preDelay_high << 12);
	//			// calculate write tap (=test tap)
	//			int rowInput = nROW;
	//			int columnInput = nCOLUMN;
	//			mWritePosition = static_cast<int>(calculateAddress(rowInput, columnInput, lastSampleRate));
	//			//round sample to 16bits
	//			float sampleRounded = mInputBuffer.getSample(channel, y);
	//			mInputBuffer.setSample(channel, y, roundBits(sampleRounded));
	//			//write sample
	//			mDelayBuffer.copyFrom(channel, mWritePosition, mInputBuffer, channel, y, 1);
	//			// calculate feedback taps
	//			delayModAddress = delayModBaseAddr + 7;
	//			delayAddress = delayBaseAddr + 16;
	//			gainModContAddress = gainModContBaseAddr + 8;
	//			gainAddress = gainBaseAddr + 8;
	//			for (int d = 0; d < 15; d++)
	//			{
	//				rowInput = U79[delayModAddress + d] + nROW;
	//				columnInput = U69[delayAddress + d * 2] + nCOLUMN;
	//				delayTaps[1 + d] = calculateAddress(rowInput, columnInput, lastSampleRate);
	//				gainModContOut = U76[gainModContAddress + d];
	//				nGainModEnable = gainModContOut >> 3;
	//				gainModContOut = gainModContOut << 5;
	//				gainModContOut = gainModContOut >> 5;
	//				gainModAddress = gainModContOut | gainModBaseAddr;
	//				gainModOut = U77[gainModAddress];
	//				gainOut = U78[gainAddress + d] << 1;
	//				if (gainModOut < gainOut && nGainModEnable == 0)
	//				{
	//					gainCeiling[1 + d] = gainModOut;
	//				}
	//				else
	//				{
	//					gainCeiling[1 + d] = gainOut;
	//				}
	//				nGSN = U78[gainAddress + d] >> 7;
	//				signMod[1 + d] = nGSN;
	//				mReadPosition = delayTaps[1 + d];
	//				if (signMod[1 + d] == 0)
	//				{
	//					mFeedbackGain = (gainCeiling[1 + d] / 256.0f) * -1.0f;
	//				}
	//				else
	//				{
	//					mFeedbackGain = (gainCeiling[1 + d] / 256.0f);
	//				}
	//				mFeedbackTaps += mDelayBuffer.getSample(0, mReadPosition) * mFeedbackGain;
	//			}
	//			mFeedbackTaps = mFeedbackTaps / 15.0f;
	//			mFeedbackBuffer.setSample(0, y, mFeedbackTaps);
	//			// calculate output taps
	//			gainAddress = gainBaseAddr + 23;
	//			delayModAddress = delayBaseAddr + 45;
	//			delayAddress = delayBaseAddr + 46;
	//			//left output taps
	//			for (int d = 0; d < 3; d++)
	//			{
	//				rowInput = U69[delayModAddress + d * 2] + nROW;
	//				columnInput = U69[delayAddress + d * 2] + nCOLUMN;
	//				delayTaps[16 + d] = calculateAddress(rowInput, columnInput, lastSampleRate);
	//				gainOut = U78[gainAddress + d] << 1;
	//				gainCeiling[16 + d] = gainOut;
	//				nGSN = U78[gainAddress + d] >> 7;
	//				signMod[16 + d] = nGSN;
	//				mReadPosition = delayTaps[16 + d];
	//				if (signMod[16 + d] == 0)
	//				{
	//					mOutputGain = (gainCeiling[16 + d] / 256.0f) * -1.0f;
	//				}
	//				else
	//				{
	//					mOutputGain = (gainCeiling[16 + d] / 256.0f);
	//				}
	//				mOutputTaps += mDelayBuffer.getSample(0, mReadPosition) * mOutputGain;
	//			}
	//			mOutputTaps = mOutputTaps / 4.0f;
	//			mOutputBuffer.setSample(0, y, mOutputTaps);
	//			//right output taps
	//			for (int d = 4; d < 8; d++)
	//			{
	//				rowInput = U69[delayModAddress + d * 2] + nROW;
	//				columnInput = U69[delayAddress + d * 2] + nCOLUMN;
	//				delayTaps[16 + d] = calculateAddress(rowInput, columnInput, lastSampleRate);
	//				gainOut = U78[gainAddress + d] << 1;
	//				gainCeiling[16 + d] = gainOut;
	//				nGSN = U78[gainAddress + d] >> 7;
	//				signMod[16 + d] = nGSN;
	//				mReadPosition = delayTaps[16 + d];
	//				if (signMod[16 + d] == 0)
	//				{
	//					mOutputGain = (gainCeiling[16 + d] / 256.0f) * -1.0f;
	//				}
	//				else
	//				{
	//					mOutputGain = (gainCeiling[16 + d] / 256.0f);
	//				}
	//				mOutputTaps += mDelayBuffer.getSample(0, mReadPosition) * mOutputGain;
	//			}
	//			mOutputTaps = mOutputTaps / 4.0f;
	//			mOutputBuffer.setSample(1, y, mOutputTaps);
	//			//process random sample
	//			float randomSample = mRandomBuffer.getSample(0, y);
	//			if (randomSample < 0)
	//			{
	//				randomSample = randomSample * -0.33f;
	//			}
	//			//scale randomSample by a certain amount
	//			randomSample = randomSample * 8.0f;
	//			//calculate rateLVL value
	//			int num1{ 0 };
	//			int num2{ 0 };
	//			int num3{ 0 };
	//			int num4{ 0 };
	//			if (randomSample >= 0.011f)
	//			{
	//				num1 = 1;
	//			}
	//			if (randomSample >= 0.0356f)
	//			{
	//				num2 = 2;
	//			}
	//			if (randomSample >= 0.0916f)
	//			{
	//				num3 = 4;
	//			}
	//			if (randomSample >= 0.317f)
	//			{
	//				num4 = 8;
	//			}
	//			rateLevel = num1 + num2 + num3 + num4;
	//			// mod rate counter
	//			modClockOut = modClockOut + 1;
	//			if (modClockOut == modRateCeiling)
	//			{
	//				modRateCount = rateLevel | (program << 4);
	//				float modClockOutScaled = U71[modRateCount] * modScale;
	//				modClockOut = modClockOutScaled;
	//			}
	//			modCarry = modClockOut + 1;
	//			if (modCarry >= modRateCeiling)
	//			{
	//				MCCK = 1;
	//			}
	//			else
	//			{
	//				MCCK = 0;
	//			}
	//			nROW = countWriteAddress(writeAddress);
	//			nCOLUMN = countWriteAddress(writeAddress) >> 8;
	//			writeAddress = countWriteAddress(writeAddress);
	//			if (MCCK == 1)
	//			{
	//				modCount = modCount + 1;
	//				if (modCount > 8191)
	//				{
	//					modCount = 0;
	//				}
	//				gainModContBaseAddr = (modCount >> 6) << 5;
	//				gainModBaseAddr = modCount << 7;
	//				gainModBaseAddr = gainModBaseAddr >> 4;
	//				delayModCount = modCount >> 6;
	//				delayModBaseAddr = delayModCount << 5;
	//			}
	//		}
	//		deEmphasis.process(juce::dsp::ProcessContextReplacing <float>(outputBlock)); //deEmphasis
	//		//add output samples to buffer
	//		buffer.copyFrom(0, 0, mOutputBuffer, 0, 0, bufferLength);
	//		buffer.copyFrom(1, 0, mOutputBuffer, 1, 0, bufferLength);
	//	}
	//	//clear input buffer
	//	mInputBuffer.clear(0, 0, bufferLength);
	//}

}

void NewProjectAudioProcessor::fillBuffer(int channel, int bufferSizeTest, int delayBufferSizeTest, float* channelData, float summingGain)
{
	if (delayBufferSizeTest > bufferSizeTest + writePositionTest)
	{
		delayBufferTest.copyFromWithRamp(channel, writePositionTest, channelData, bufferSizeTest, summingGain, summingGain);
	}
	else
	{
		auto numSamplesToEnd = delayBufferSizeTest - writePositionTest;
		delayBufferTest.copyFromWithRamp(channel, writePositionTest, channelData, numSamplesToEnd, summingGain, summingGain);
		auto numSamplesAtStart = bufferSizeTest - numSamplesToEnd;
		delayBufferTest.copyFromWithRamp(channel, 0, channelData + numSamplesToEnd, numSamplesAtStart, summingGain, summingGain);
	}
}

void NewProjectAudioProcessor::readFromBuffer(int delayBufferSizeTest, int bufferSizeTest, int channel, juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBufferTest, float outputDelayTime, float outputDelayGain)
{
	auto readPositionTest = static_cast<int>(writePositionTest - (getSampleRate() * outputDelayTime));
	if (readPositionTest < 0)
	{
		readPositionTest += delayBufferSizeTest;
	}
	if (readPositionTest + bufferSizeTest < delayBufferSizeTest)
	{
		buffer.addFromWithRamp(channel, 0, delayBufferTest.getReadPointer(channel, readPositionTest), bufferSizeTest, outputDelayGain, outputDelayGain);
	}
	else
	{
		auto numSamplesToEnd = delayBufferSizeTest - readPositionTest;
		buffer.addFromWithRamp(channel, 0, delayBufferTest.getReadPointer(channel, readPositionTest), numSamplesToEnd, outputDelayGain, outputDelayGain);
		auto numSamplesAtStart = bufferSizeTest - numSamplesToEnd;
		buffer.addFromWithRamp(channel, numSamplesToEnd, delayBufferTest.getReadPointer(channel, 0), numSamplesAtStart, outputDelayGain, outputDelayGain);
	}
}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
	return new NewProjectAudioProcessorEditor(*this);
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void NewProjectAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
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

juce::AudioProcessorValueTreeState::ParameterLayout NewProjectAudioProcessor::createParameters()
{
	std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

	parameters.push_back(std::make_unique<juce::AudioParameterChoice>("PROGRAM", "Program",
		juce::StringArray("Plate 1", "Plate 2", "Chamber", "Small Hall", "Hall", "Large Hall", "Cathedral", "Canyon"), 0));

	return { parameters.begin(), parameters.end() };

}