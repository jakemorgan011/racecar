/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

termite::stripe s1(0,1);
termite::stripe s2(30,1);
termite::stripe s3(60,1);

//==============================================================================
FletchAudioProcessorEditor::FletchAudioProcessorEditor (FletchAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    customLookAndFeel.setReferences();
    setLookAndFeel(&customLookAndFeel);
    colours = customLookAndFeel.getColours();
    
    //
    stripes = {&s1, &s2, &s3};
    stripes[0]->setColour(*colours[2]);
    stripes[1]->setColour(*colours[3]);
    stripes[2]->setColour(*colours[4]);
    timer.setStripes(stripes);
    timer.startTimer(5);
    addAndMakeVisible(*stripes[0]);
    addAndMakeVisible(*stripes[1]);
    addAndMakeVisible(*stripes[2]);
    
    //
    scroll.setText(tag);
    scroll.setColour(*colours[1]);
    sTimer.setScroll(&scroll);
    sTimer.startTimer(80);
    addAndMakeVisible(scroll);
    
    //
    distortionSlider.setRange(0.f,1.0f);
    distortionSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    distortionSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0,0);
    addAndMakeVisible(distortionSlider);
    typeSlider.setRange(0.f,1.0f);
    typeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    typeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0,0);
    addAndMakeVisible(typeSlider);
    dryWetSlider.setRange(0.f,1.0f);
    dryWetSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    dryWetSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0,0);
    addAndMakeVisible(dryWetSlider);
    
    //
    setResizable(false, false);
    setSize (500, 200);
}

FletchAudioProcessorEditor::~FletchAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void FletchAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.setColour(*colours[0]);
    g.fillAll();
}

void FletchAudioProcessorEditor::resized()
{
    stripes[0]->setBounds(0, 50, getWidth(), 50);
    stripes[1]->setBounds(0, 100, getWidth(), 50);
    stripes[2]->setBounds(0, 150, getWidth(), 50);
    scroll.setBounds(350,150,150,50);
    distortionSlider.setBounds(10,10,100,100);
    typeSlider.setBounds(160, 10, 100, 100);
    dryWetSlider.setBounds(310, 10, 100, 100);
    
}

void FletchAudioProcessorEditor::changePallette(){
    
}
