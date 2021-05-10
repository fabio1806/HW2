/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SubSynthAudioProcessor::SubSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
                        
#endif
{
}

SubSynthAudioProcessor::~SubSynthAudioProcessor()
{
}

//==============================================================================
const juce::String SubSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SubSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SubSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SubSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SubSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SubSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SubSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SubSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SubSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void SubSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SubSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    lastSampleRate = sampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    // Initialize and reset the filter
    filter.prepare (spec);
    filter.reset();
    
    // Initialize and reset the oscillator
    for (auto& oscillator : oscillators){
        oscillator.setFrequency(440.0f);
        oscillator.prepare(spec);
        oscillator.reset();
    }
    
    // Initialize and reset the gain
    gain.setGainLinear(0.0f);
    
}

void SubSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SubSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SubSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Use AudioBlock as a container for the input buffer
    // Use ProcessContextReplacing to create a buffer used for all processes
    // connected to the AudioBlock object

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    
    // Get parameters from the pointer in the ValueTreeState
    float cutoff = *apvts.getRawParameterValue("CUTOFF");
    float quality = *apvts.getRawParameterValue("Q");
    float amplitude = *apvts.getRawParameterValue("AMPL");
    
    filterMenu = *apvts.getRawParameterValue("FILTER");
    
    bool sine = *apvts.getRawParameterValue("SINE");
    bool square = *apvts.getRawParameterValue("SQUARE");
    bool triangle = *apvts.getRawParameterValue("TRIANGLE");
    bool saw = *apvts.getRawParameterValue("SAW");
    
    //******************************************************************************
    // Implement the actual Subtractive Synthesis
    // Retrieve the note value from buffer messages
    juce::MidiMessage m;
    int time;
        
    for (juce::MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        if (m.isNoteOn())
        {
            amp = amplitude;
            freq = m.getMidiNoteInHertz(m.getNoteNumber());
        }
        else if (m.isNoteOff())
        {
            amp = 0;
        }
    }
    
    // Apply the formula block by block:
    // 1) Oscillator and amplifier
    
    
    updateParametersOsc(freq, amp);
        
    if (sine){
        oscillators[0].process(context);
        gain.process(context);
    }
    else if (square){
        oscillators[1].process(context);
        gain.process(context);
    }
    else if (triangle){
        oscillators[2].process(context);
        gain.process(context);
    }
    else if (saw){
        oscillators[3].process(context);
        gain.process(context);
    }
    
    // 2) Filter
    // Update the filter state and process it to context block
    updateParametersFilter(lastSampleRate, cutoff, quality);
    filter.process(context);

    //******************************************************************************
    
}

//==============================================================================
bool SubSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SubSynthAudioProcessor::createEditor()
{
    return new SubSynthAudioProcessorEditor (*this);
}

//==============================================================================
void SubSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SubSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SubSynthAudioProcessor();
}

//******************************************************************************
// Implement the function updateParameters
void SubSynthAudioProcessor::updateParametersFilter(float sampleRate, float cutoff, float quality)
{
    if (sampleRate != 0.0)
    {
        filter.setCutoffFrequency(cutoff);
        filter.setResonance(quality);

        setType();
    }
}

void SubSynthAudioProcessor::updateParametersOsc(float freq, float amp)
{
    for (auto&& oscillator : oscillators)
        oscillator.setFrequency(freq);
    
    gain.setGainLinear(amp/100.0f);
}

// Implement the function createParameters
juce::AudioProcessorValueTreeState::ParameterLayout SubSynthAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
    
    // Push in the vector all objects using unique pointer
    parameters.push_back (std::make_unique<juce::AudioParameterFloat>("CUTOFF", "Cutoff Frequency", 20.0f, 20000.0f, 440.0f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat>("Q", "Q factor Frequency", 0.1f, 20.0f, 0.1f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat>("AMPL", "Amplitude Modulation factor", 0.0f, 100.0f, 0.0f));
    parameters.push_back (std::make_unique<juce::AudioParameterInt>("FILTER", "Filter Type", 1, 3, 1));
    
    parameters.push_back (std::make_unique<juce::AudioParameterBool>("SINE", "Sine Waveform", true));
    parameters.push_back (std::make_unique<juce::AudioParameterBool>("SQUARE", "Square Waveform", false));
    parameters.push_back (std::make_unique<juce::AudioParameterBool>("TRIANGLE", "Triangle Waveform", false));
    parameters.push_back (std::make_unique<juce::AudioParameterBool>("SAW", "Saw Waveform", false));
    
    return { parameters.begin(), parameters.end() };
}


// Implement functions used to initialize and update the filter
void SubSynthAudioProcessor::setType()
{
    using fType = juce::dsp::StateVariableTPTFilterType;

    switch (filterMenu)
    {
        case 1:
            filter.setType (fType::lowpass);
	        break;

        case 2:
            filter.setType (fType::bandpass);
	        break;

        case 3:
            filter.setType (fType::highpass);
	        break;

	    default:
	        filter.setType (fType::lowpass);
	        break;
    }
}
