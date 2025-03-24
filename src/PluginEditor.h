/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "customLookAndFeel.h"

// okay since they have custom contructors you have to do this.
extern termite::stripe s1;
extern termite::stripe s2;
extern termite::stripe s3;

//==============================================================================
/**
*/
class FletchAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    FletchAudioProcessorEditor (FletchAudioProcessor&);
    ~FletchAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void changePallette();

private:
    customLookAndFeel customLookAndFeel;
    juce::Slider distortionSlider;
    juce::Slider typeSlider;
    juce::Slider dryWetSlider;
    std::vector<juce::Colour*> colours;
    std::vector<termite::stripe*> stripes;
    timer timer;
    scrollTimer sTimer;
    // the fake text loop lol
    std::string tag = "  racecar made by public services <3  racecar made";
    termite::textScroll scroll;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FletchAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FletchAudioProcessorEditor)
};
