#include <iostream>
#include "PluginProcessor.h"
#include <JuceHeader.h>
#include "PluginEditor.h"
#include <Jonssonic/utils/BufferUtils.h>


EQAudioProcessor::EQAudioProcessor()
    : parameterManager(EQParams::createParams(), *this)
{
// ============================================================================
// [DEBUG]: Prints all APVTS parameter IDs at startup
// This helps verify that your parameters are registered correctly.
// You can remove or comment out this block when you no longer need it.
auto& apvts = parameterManager.getAPVTS();
DBG("[DEBUG] APVTS parameter IDs at startup:");
for (auto* param : apvts.processor.getParameters()) {
    if (auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(param)) {
        DBG("  " + paramWithID->paramID);
    }
}
// ============================================================================

    // Register callbacks for parameter changes
    using ID = EQParams::ID;
    
    parameterManager.on(ID::LowCutFreq, [this](float value, bool skipSmoothing) {
        // Update low cut filter frequency
    });

    parameterManager.on(ID::LowMidGain, [this](float value, bool skipSmoothing) {
        // Update low mid gain
    });

    parameterManager.on(ID::HighMidGain, [this](float value, bool skipSmoothing) {
        // Update high mid gain
    });

    parameterManager.on(ID::HighShelfGain, [this](float value, bool skipSmoothing) {
        // Update high shelf gain
    });

    parameterManager.on(ID::LowMidFreq, [this](float value, bool skipSmoothing) {
        // Update low mid frequency
    });

    parameterManager.on(ID::HighMidFreq, [this](float value, bool skipSmoothing) {
        // Update high mid frequency
    });
}

EQAudioProcessor::~EQAudioProcessor()
{
}

void EQAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());
    // Prepare all DSP objects and buffers here

    
    // Initialize DSP with parameter defaults (defined in Params.h) (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void EQAudioProcessor::releaseResources()
{
    // Release DSP resources here

}

void EQAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Get audio buffer info
    const int numInputChannels = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Early return if no audio to process
    if (numInputChannels == 0 || numOutputChannels == 0 || numSamples == 0)
        return;
    // Update all parameters from FIFO (GUI thread → Audio thread)
    parameterManager.update();

    // Handle denormals
    juce::ScopedNoDenormals noDenormals;

    // Note: Jonssonic DSP expects numInputChannels == numOutputChannels
    // So we map the input channels to output channels accordingly
    Jonssonic::mapChannels<float>(
        buffer.getArrayOfReadPointers(), 
        buffer.getArrayOfWritePointers(), 
        numInputChannels, 
        numOutputChannels, 
        numSamples);

    
    // Process the EQ DSP here


}

void EQAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // This is taken care of by the parameter manager automatically
    parameterManager.saveState(destData);
}

void EQAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // This is taken care of by the parameter manager automatically
    parameterManager.loadState(data, sizeInBytes);
}

bool EQAudioProcessor::acceptsMidi() const
{
    return true;
}




//==============================================================================
const juce::String EQAudioProcessor::getName() const { return JucePlugin_Name; }
bool EQAudioProcessor::producesMidi() const { return false; }
bool EQAudioProcessor::isMidiEffect() const { return false; }
double EQAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int EQAudioProcessor::getNumPrograms() { return 1; }
int EQAudioProcessor::getCurrentProgram() { return 0; }
void EQAudioProcessor::setCurrentProgram(int) { }
const juce::String EQAudioProcessor::getProgramName(int) { return {}; }
void EQAudioProcessor::changeProgramName(int, const juce::String&) { }
bool EQAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* EQAudioProcessor::createEditor() { return new EQAudioProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EQAudioProcessor();
}