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

//******************************************************************************
// Add some constants

#define SAMPLE_RATE  (44100)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
//******************************************************************************

class SubSynthAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SubSynthAudioProcessor();
    ~SubSynthAudioProcessor() override;

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
    
    //******************************************************************************
    // Declare a function that implements the communication between Editor and Processor
    // by AudioProcessorValueTreeState class
    
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    //******************************************************************************
    
private:
    //==============================================================================
    
    // Add the TPT filter and DSP Oscillator object
    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::dsp::Gain<float> gain;

    juce::dsp::Oscillator<float> oscillators[4] = {
        {[] (float x) { return std::sin (x); }},                                                // sine
        {[] (float x) { return x < 0.0f ? -1.0f : 1.0f; }},                                     // square
        {[] (float x) { return x < 0.0f ? (-2.0 * x / M_PI - 1.0) : (2.0 * x / M_PI - 1.0);}},  // triagle
        {[] (float x) { return x / M_PI; }},                                                    // saw
    };
    
    // Declare all functions used
    void setType();
    void updateParametersFilter(float sampleRate, float cutoff, float quality);
    void updateParametersOsc(float freq, float amp);
    
    // Declare all the variable we are going to use for the computation
    float freq;
    float amp;
    float lastSampleRate;
    int filterMenu;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SubSynthAudioProcessor)
};
