/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SubSynthAudioProcessorEditor  :   public juce::AudioProcessorEditor
{
public:
    SubSynthAudioProcessorEditor (SubSynthAudioProcessor&);
    ~SubSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SubSynthAudioProcessor& audioProcessor;

    //*************************************************************************
    
    juce::Slider cutoff;
    juce::Label cutoffLabel;
    
    juce::Slider amplitude;
    juce::Label amplitudeLabel;
    
    juce::Slider quality;
    juce::Label qualityLabel;
    
    juce::GroupComponent wave;
    
    juce::ToggleButton sine;
    juce::ToggleButton square;
    juce::ToggleButton saw;
    juce::ToggleButton triangle;
    
    juce::Label filterLabel;
    juce::ComboBox filterMenu;
    
    // Create a unique pointer for each slider
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qualitySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amplitudeSliderAttachment;
    
    // Create a unique pointer for the combobox object
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterMenuAttachment;
    
    // Create a unique pointer for each button
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> sineButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> squareButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> sawButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> triangleButtonAttachment;
    
    //*************************************************************************
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SubSynthAudioProcessorEditor)
};
