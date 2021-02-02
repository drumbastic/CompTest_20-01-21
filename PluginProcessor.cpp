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
    *(state = std::make_unique<juce::AudioProcessorValueTreeState>(*this, nullptr));
    *(comp  = std::make_unique<juce::dsp::Compressor<float>>());
    *(gain  = std::make_unique<juce::dsp::Gain<float>>());

    state->createAndAddParameter("threshold", "Threshold", "Threshold", juce::NormalisableRange<float>(-50.f, 0.0f, 5.f), -40.0, nullptr, nullptr);
    state->createAndAddParameter("ratio", "Ratio", "Ratio", juce::NormalisableRange<float>(1.0f, 30.0f, 3.f), 15.0, nullptr, nullptr);
    state->createAndAddParameter("attack", "Attack", "Attack", juce::NormalisableRange<float>(0.0f, 20.0f, 0.1), 10.0, nullptr, nullptr);
    state->createAndAddParameter("release", "Release", "Release", juce::NormalisableRange<float>(0.0f, 200.0f, 0.1), 100.0, nullptr, nullptr);
    state->createAndAddParameter("gain", "Gain", "Gain", juce::NormalisableRange<float>(-100.0f, 0.0f, 1.f), -20.0, nullptr, nullptr);
    state->state = juce::ValueTree("threshold");
    state->state = juce::ValueTree("ratio");
    state->state = juce::ValueTree("attack");
    state->state = juce::ValueTree("release");
    state->state = juce::ValueTree("gain");
 
    comp->setThreshold(-40.0);
    comp->setRatio(15.0);
    comp->setAttack(10.0f);
    comp->setRelease(100.0f);

    gain->setGainDecibels(10.0f);

    oversampAmount = 1;
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

    std::unique_ptr<juce::dsp::ProcessSpec> spec = std::make_unique< juce::dsp::ProcessSpec>();

    spec->sampleRate = sampleRate * oversampAmount; //set by oversample button value
    spec->maximumBlockSize = samplesPerBlock * oversampAmount; //set by oversample button value
    spec->numChannels = getTotalNumInputChannels();

    comp.reset(new juce::dsp::Compressor<float>);
    gain.reset(new juce::dsp::Gain<float>);
    oversamp.reset(new juce::dsp::Oversampling<float>(getNumOutputChannels(), 2, juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple, false));

    comp->prepare(*spec.get());
    gain->prepare(*spec.get()); 
    oversamp->initProcessing((size_t)samplesPerBlock);
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

void NewProjectAudioProcessor::updateParameters()
{
    // Update your parameters and your processes here
    float threshold = *state->getRawParameterValue("threshold");
    float ratio = *state->getRawParameterValue("ratio");
    float attack = *state->getRawParameterValue("attack");
    float release = *state->getRawParameterValue("release");
    float gn = *state->getRawParameterValue("gain");

    comp->setThreshold(threshold);
    comp->setRatio(ratio);
    comp->setAttack(attack);
    comp->setRelease(release);

    gain->setGainDecibels(gn);
}


void NewProjectAudioProcessor::process(juce::dsp::ProcessContextReplacing<float> context)
{
    // Do processing here and output
    updateParameters();

    gain->process(context);
    comp->process(context);
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

    
    // Input to dsp module
    juce::dsp::AudioBlock<float> block (buffer);

    if (filteringEnabled) 
    {
        oversampAmount = 2;
        juce::dsp::AudioBlock<float> osBlock = oversamp->processSamplesUp(block);
        process(juce::dsp::ProcessContextReplacing<float>(osBlock));
        oversamp->processSamplesDown(block);
    }
    else 
    {
        // Output from dsp module
        oversampAmount = 1;
        process(juce::dsp::ProcessContextReplacing<float>(block));
    }
    
}

juce::AudioProcessorValueTreeState& NewProjectAudioProcessor::getState()
{
    return *state;
}

void NewProjectAudioProcessor::setFilteringEnbaled(const bool shouldBeEnabled)
{
    filteringEnabled = shouldBeEnabled;
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

    juce::MemoryOutputStream stream(destData, false);
    state->state.writeToStream(stream);
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    juce::ValueTree tree = juce::ValueTree::readFromData(data, sizeInBytes);

    if (tree.isValid())
    {
        state->state = tree;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}
