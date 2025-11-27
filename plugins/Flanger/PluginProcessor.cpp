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
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());
    flanger.prepare(numChannels, sampleRate, 10.0f);
    fxBuffer.setSize(numChannels, samplesPerBlock);
    dryWetMixer.prepare(numChannels, sampleRate);
}

void FlangerAudioProcessor::releaseResources()
{
    flanger.clear();
    dryWetMixer.reset();
}

void FlangerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    juce::ScopedNoDenormals noDenormals;

    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // Copy input to fxBuffer for processing
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        if (channel < totalNumInputChannels)
            fxBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
        else
            fxBuffer.copyFrom(channel, 0, buffer, 0, 0, numSamples); // Duplicate first channel
    }

    // Process wet signal in fxBuffer
    flanger.processBlock(fxBuffer.getArrayOfReadPointers(),
                         fxBuffer.getArrayOfWritePointers(),
                         static_cast<size_t>(numSamples));
    
    // Mix wet signal with dry (addFrom adds wet to existing dry signal)
    dryWetMixer.processBlock(buffer.getArrayOfReadPointers(),   // dry input
                             fxBuffer.getArrayOfReadPointers(), // wet input
                             buffer.getArrayOfWritePointers(),  // output
                             static_cast<size_t>(numSamples));  // number of samples
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