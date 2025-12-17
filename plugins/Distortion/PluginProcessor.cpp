#include <iostream>
#include "PluginProcessor.h"
#include <JuceHeader.h>
#include "PluginEditor.h"
#include <Jonssonic/utils/BufferUtils.h>


DistortionAudioProcessor::DistortionAudioProcessor()
    : parameterManager(DistortionParams::createParams(), *this)
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
    using ID = DistortionParams::ID;
    
    parameterManager.on(ID::Mix, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Mix changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        // Call your DSP mix setter here
        dryWetMixer.setMix(value * 0.01f); // We are converting from [0,100] to [0,1]
        
    });
    parameterManager.on(ID::Enable, [this](bool value, bool skipSmoothing) {
        DBG("[DEBUG] Enable changed: " + juce::String(value ? "true" : "false") + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        // Call your DSP enable setter here 
        
    });
    parameterManager.on(ID::Mode, [this](int value, bool skipSmoothing) {
        DBG("[DEBUG] Mode changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        // Call your DSP mode setter here
    });


}

DistortionAudioProcessor::~DistortionAudioProcessor()
{
}

void DistortionAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());
    // Prepare all DSP objects and buffers here
    dryWetMixer.prepare(numChannels, static_cast<float>(sampleRate));
    fxBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    
    // Initialize DSP with parameter defaults (defined in Params.h) (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void DistortionAudioProcessor::releaseResources()
{
    // Release DSP resources here
    dryWetMixer.reset();
    fxBuffer.setSize(0, 0);
}

void DistortionAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
        fxBuffer.getArrayOfWritePointers(), 
        numInputChannels, 
        numOutputChannels, 
        numSamples);

    
    // DSP processing here (this template includes only a dry/wet mixer as an example)
    dryWetMixer.processBlock(buffer.getArrayOfReadPointers(),   // dry input
                             fxBuffer.getArrayOfReadPointers(), // wet input
                             buffer.getArrayOfWritePointers(),  // output
                             static_cast<size_t>(numSamples));  // number of samples
}

void DistortionAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // This is taken care of by the parameter manager automatically
    parameterManager.saveState(destData);
}

void DistortionAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // This is taken care of by the parameter manager automatically
    parameterManager.loadState(data, sizeInBytes);
}

bool DistortionAudioProcessor::acceptsMidi() const
{
    return true;
}




//==============================================================================
const juce::String DistortionAudioProcessor::getName() const { return JucePlugin_Name; }
bool DistortionAudioProcessor::producesMidi() const { return false; }
bool DistortionAudioProcessor::isMidiEffect() const { return false; }
double DistortionAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int DistortionAudioProcessor::getNumPrograms() { return 1; }
int DistortionAudioProcessor::getCurrentProgram() { return 0; }
void DistortionAudioProcessor::setCurrentProgram(int) { }
const juce::String DistortionAudioProcessor::getProgramName(int) { return {}; }
void DistortionAudioProcessor::changeProgramName(int, const juce::String&) { }
bool DistortionAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* DistortionAudioProcessor::createEditor() { return new DistortionAudioProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionAudioProcessor();
}