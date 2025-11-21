#include "PluginProcessor.h"
#include <JuceHeader.h>
#include "PluginEditor.h"


FlangerAudioProcessor::FlangerAudioProcessor()
    
{

}

FlangerAudioProcessor::~FlangerAudioProcessor()
{
}

void FlangerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{

}

void FlangerAudioProcessor::releaseResources()
{
}

void FlangerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    juce::ScopedNoDenormals noDenormals;

    const unsigned int numChannels{ static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples{ static_cast<unsigned int>(buffer.getNumSamples()) };


}

void FlangerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{

}

void FlangerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{

}

bool FlangerAudioProcessor::acceptsMidi() const
{
    return true;
}




//==============================================================================
const juce::String FlangerAudioProcessor::getName() const { return JucePlugin_Name; }
bool FlangerAudioProcessor::producesMidi() const { return false; }
bool FlangerAudioProcessor::isMidiEffect() const { return false; }
double FlangerAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int FlangerAudioProcessor::getNumPrograms() { return 1; }
int FlangerAudioProcessor::getCurrentProgram() { return 0; }
void FlangerAudioProcessor::setCurrentProgram(int) { }
const juce::String FlangerAudioProcessor::getProgramName(int) { return {}; }
void FlangerAudioProcessor::changeProgramName(int, const juce::String&) { }
bool FlangerAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* FlangerAudioProcessor::createEditor() { return new FlangerAudioProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FlangerAudioProcessor();
}