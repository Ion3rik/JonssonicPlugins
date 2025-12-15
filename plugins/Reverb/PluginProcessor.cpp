#include <iostream>
#include "PluginProcessor.h"
#include <JuceHeader.h>
#include "PluginEditor.h"
#include <Jonssonic/utils/BufferUtils.h>


ReverbAudioProcessor::ReverbAudioProcessor()
    : parameterManager(ReverbParams::createParams(), *this)
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
    using ID = ReverbParams::ID;

    parameterManager.on(ID::ReverbTime,[this](float newValue, bool skipSmoothing) {
        // Update your DSP object's reverb time here
        // e.g., reverb.setReverbTime(newValue);
    });

    parameterManager.on(ID::Size,[this](float newValue, bool skipSmoothing) {
        // Update your DSP object's size here
        // e.g., reverb.setSize(newValue);
    });

    parameterManager.on(ID::PreDelay,[this](float newValue, bool skipSmoothing) {
        // Update your DSP object's pre-delay here
        // e.g., reverb.setPreDelay(newValue);
    });

    parameterManager.on(ID::LowCut,[this](float newValue, bool skipSmoothing) {
        // Update your DSP object's low cut here
        // e.g., reverb.setLowCut(newValue);
    });

    parameterManager.on(ID::Damping,[this](float newValue, bool skipSmoothing) {
        // Update your DSP object's damping here
        // e.g., reverb.setDamping(newValue);
    });
    
    parameterManager.on(ID::Mix,[this](float newValue, bool skipSmoothing) {
        dryWetMixer.setMix(newValue / 100.0f); // Convert percentage to [0.0, 1.0]
    });


}

ReverbAudioProcessor::~ReverbAudioProcessor()
{
}

void ReverbAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());
    // Prepare all DSP objects and buffers here
    dryWetMixer.prepare(numChannels, static_cast<float>(sampleRate));
    fxBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    
    // Initialize DSP with parameter defaults (defined in Params.h) (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void ReverbAudioProcessor::releaseResources()
{
    // Release DSP resources here
    dryWetMixer.reset();
    fxBuffer.setSize(0, 0);
}

void ReverbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

void ReverbAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // This is taken care of by the parameter manager automatically
    parameterManager.saveState(destData);
}

void ReverbAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // This is taken care of by the parameter manager automatically
    parameterManager.loadState(data, sizeInBytes);
}

bool ReverbAudioProcessor::acceptsMidi() const
{
    return true;
}




//==============================================================================
const juce::String ReverbAudioProcessor::getName() const { return JucePlugin_Name; }
bool ReverbAudioProcessor::producesMidi() const { return false; }
bool ReverbAudioProcessor::isMidiEffect() const { return false; }
double ReverbAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int ReverbAudioProcessor::getNumPrograms() { return 1; }
int ReverbAudioProcessor::getCurrentProgram() { return 0; }
void ReverbAudioProcessor::setCurrentProgram(int) { }
const juce::String ReverbAudioProcessor::getProgramName(int) { return {}; }
void ReverbAudioProcessor::changeProgramName(int, const juce::String&) { }
bool ReverbAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* ReverbAudioProcessor::createEditor() { return new ReverbAudioProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverbAudioProcessor();
}