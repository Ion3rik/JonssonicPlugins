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
}

void FlangerAudioProcessor::releaseResources()
{
    flanger.clear();
}

void FlangerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    juce::ScopedNoDenormals noDenormals;

    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // Copy first input channel to all output channels if there are fewer input channels
    if (totalNumInputChannels > 0 && totalNumInputChannels < totalNumOutputChannels)
    {
        const float* firstChannelData = buffer.getReadPointer(0);
        
        for (int channel = totalNumInputChannels; channel < totalNumOutputChannels; ++channel)
        {
            buffer.copyFrom(channel, 0, firstChannelData, numSamples);
        }
    }

    flanger.processBlock(buffer.getArrayOfReadPointers(),
                         buffer.getArrayOfWritePointers(),
                         static_cast<size_t>(numSamples));
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