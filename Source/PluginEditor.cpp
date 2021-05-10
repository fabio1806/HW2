/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SubSynthAudioProcessorEditor::SubSynthAudioProcessorEditor (SubSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 200);
    
    //*************************************************************************
    
    cutoff.setRange(20.00, 20000.00, 0.01);
    cutoff.setSliderStyle(juce::Slider::LinearHorizontal);
    cutoff.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    
    cutoffLabel.setText("Cutoff", juce::dontSendNotification);
    addAndMakeVisible(cutoff);
    addAndMakeVisible(cutoffLabel);
    
    quality.setRange(0.10, 20.00, 0.01);
    quality.setSliderStyle(juce::Slider::LinearHorizontal);
    quality.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    
    qualityLabel.setText("Q", juce::dontSendNotification);
    addAndMakeVisible(quality);
    addAndMakeVisible(qualityLabel);
    
    amplitude.setRange(0.00, 100.00, 0.01);
    amplitude.setSliderStyle(juce::Slider::LinearHorizontal);
    amplitude.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    
    amplitudeLabel.setText("Amplitude", juce::dontSendNotification);
    addAndMakeVisible(amplitude);
    addAndMakeVisible(amplitudeLabel);
    
    wave.setName("group");
    wave.setText("Waveform");
    addAndMakeVisible(wave);
    
    sine.setRadioGroupId(1234);
    sine.setButtonText("Sine wave");
    sine.setTooltip("Sine waveform oscillator");
    sine.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(sine);
    
    square.setRadioGroupId(1234);
    square.setButtonText("Square wave");
    square.setTooltip("Square waveform oscillator");
    addAndMakeVisible(square);
    
    saw.setRadioGroupId(1234);
    saw.setButtonText("Saw wave");
    saw.setTooltip("Sawtooth waveform oscillator");
    addAndMakeVisible(saw);
    
    triangle.setRadioGroupId(1234);
    triangle.setButtonText("Triangle wave");
    triangle.setTooltip("Triangle waveform oscillator");
    addAndMakeVisible(triangle);
    
    filterLabel.setText("Filter type", juce::dontSendNotification);
    addAndMakeVisible(filterLabel);
    
    filterMenu.addItem("Low-Pass", 1);
    filterMenu.addItem("Band-Pass", 2);
    filterMenu.addItem("High-Pass", 3);
    addAndMakeVisible(filterMenu);
    filterMenu.setSelectedId (1);
    
    // Instantiate our attachments and link the value tree state
    cutoffSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "CUTOFF", cutoff);
    qualitySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Q", quality);
    amplitudeSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "AMPL", amplitude);
    
    filterMenuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "FILTER", filterMenu);
    
    sineButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "SINE", sine);
    squareButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "SQUARE", square);
    sawButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "SAW", saw);
    triangleButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "TRIANGLE", triangle);
    //*************************************************************************
}

SubSynthAudioProcessorEditor::~SubSynthAudioProcessorEditor()
{
}

//==============================================================================
void SubSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void SubSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    cutoff.setBounds(290, 70, 200, 20);
    cutoffLabel.setBounds(200, 70, 80, 20);
    
    quality.setBounds(290, 100, 200, 20);
    qualityLabel.setBounds(200, 100, 80, 20);
    
    amplitude.setBounds(290, 160, 200, 20);
    amplitudeLabel.setBounds(200, 160, 80, 20);
    
    wave.setBounds(10, 20, 180, 170);
    
    sine.setBounds(20, 40, 150, 20);
    square.setBounds(20, 70, 150, 20);
    saw.setBounds(20, 100, 150, 20);
    triangle.setBounds(20, 130, 150, 20);
    
    filterLabel.setBounds(200, 40, 80, 20);
    filterMenu.setBounds(290, 40, 200, 20);
    
}
