/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FletchAudioProcessor::FletchAudioProcessor()
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
    _constructValueTreeState();
    distortion = ValueTreeState->getRawParameterValue("distortion");
    type = ValueTreeState->getRawParameterValue("type");
    drywet = ValueTreeState->getRawParameterValue("drywet");
}

FletchAudioProcessor::~FletchAudioProcessor()
{
}

//==============================================================================
const juce::String FletchAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FletchAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FletchAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FletchAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FletchAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FletchAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FletchAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FletchAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FletchAudioProcessor::getProgramName (int index)
{
    return {};
}

void FletchAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FletchAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lowShelf.prepareToPlay(sampleRate);
}

void FletchAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FletchAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void FletchAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    //TODO: THIS IS IT
    //THIS IS WHAT HAS BEEN CAUSING THE VALIDATION TO CRASH
    // YOU NEED TO SET THE SIZE OF YOUR BUFFERS IN SYNC WITH THE MAIN BUFFER SO THEY ARE INTERACTING IN IDENTICAL SPACES
    dryBuff.setSize(getTotalNumInputChannels(), buffer.getNumSamples());
    wetBuff.setSize(getTotalNumInputChannels(), buffer.getNumSamples());
    // this comes with the juce default code.
    // im keeping this because this could potentially be where the memory leak is.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i){
        //TODO: THIS IS ALSO IT...
        // MAKE SURE YOU ARE CLEARING YOUR BUFFERS SO THAT THERE ISN'T JUNK INSIDE OF THEM.
        buffer.clear (i, 0, buffer.getNumSamples());
        dryBuff.clear(i, 0, buffer.getNumSamples());
        wetBuff.clear(i, 0, buffer.getNumSamples());
    }
    
    // this also comes with the juce default.
    // gonna try to work within it to see if this fixes my memory leak.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto* wetChannelData = wetBuff.getWritePointer(channel);
        auto* dryChannelData = dryBuff.getWritePointer(channel);
        auto num_samples = buffer.getNumSamples();
        for(int i = 0; i < num_samples; i++){
            int tp = *type;
            float dry_wet = *drywet;
            float dist = *distortion;
            dryChannelData[i] = channelData[i] * (1 - *drywet);
            // would change this if i wanted it dynamic...
            lowShelf.makeLowShelf(200, 2);
            switch(tp){
                case 0:
                    wetChannelData[i] = lowShelf.filterSample(wetChannelData[i]);
                    wetChannelData[i] = tanh((channelData[i]*(dist*20)));
                    break;
                                
                case 1:
                    // soft clip
                    wetChannelData[i] = tanh((channelData[i]*(dist*10)));
                    break;
                                
                                // misc algorithms
                case 2:
                    wetChannelData[i] = tanh(channelData[i]*tan(channelData[i]*(dist*10))*(dist*10));
                    break;
                                
                                // this does nothing lol
                case 3:
                    wetChannelData[i] = tanh(channelData[i]*(sin(channelData[i])/tan(channelData[i]*(dist*2))*dist*100));
                    break;
            }
            wetChannelData[i] *= dry_wet;
            channelData[i] = wetChannelData[i] + dryChannelData[i];
        }
    }
}

//==============================================================================
bool FletchAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FletchAudioProcessor::createEditor()
{
    return new FletchAudioProcessorEditor (*this);
}

//==============================================================================
void FletchAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FletchAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FletchAudioProcessor();
}


void FletchAudioProcessor::_constructValueTreeState(){
    ValueTreeState.reset(new juce::AudioProcessorValueTreeState(*this, nullptr, juce::Identifier("racecar"),{
        
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("distortion", 1), "distortion", juce::NormalisableRange<float>(0.0f,1.f,0.01f), 0.0f),
        
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("type", 1), "type", juce::NormalisableRange<float>(0.0f,3.f,1.0f), 0.f),
        
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("drywet",1), "drywet", juce::NormalisableRange<float>(0.0f,1.f,0.01f), 0.5f)
    }));
}
