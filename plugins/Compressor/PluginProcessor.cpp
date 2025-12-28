#include <iostream>
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <Jonssonic/utils/BufferUtils.h>


CompressorAudioProcessor::CompressorAudioProcessor()
    : parameterManager(CompressorParams::createParams(), *this),
    visualizerManager(CompressorVisualizers::createVisualizers())
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
    using ID = CompressorParams::ID;
    
    parameterManager.on(ID::Threshold, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Threshold changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        // Call your DSP threshold setter here
        compressor.setThreshold(value, skipSmoothing);
    });

    parameterManager.on(ID::Attack, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Attack changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        // Call your DSP attack setter here
        compressor.setAttack(value, skipSmoothing);
    });

    parameterManager.on(ID::Release, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Release changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        // Call your DSP release setter here
        compressor.setRelease(value, skipSmoothing);
    });

    parameterManager.on(ID::Ratio, [this](int value, bool skipSmoothing) {
        DBG("[DEBUG] Ratio changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        // Call your DSP ratio setter here
        compressor.setRatio(static_cast<float>(value), skipSmoothing);
    });

    parameterManager.on(ID::Mix, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Mix changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        // Call your DSP mix setter here
        compressor.setMix(value * 0.01, skipSmoothing);

        
    });

    parameterManager.on(ID::Output, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Output changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        // Call your DSP output gain setter here
        compressor.setOutputGain(value, skipSmoothing);
    });

    parameterManager.on(ID::CharacterMode, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Character Mode changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        // Call your DSP character mode setter here
        compressor.setCharacterMode(value > 0.5f, skipSmoothing);
    });

    // Register visualizer value suppliers
    using VisualizerID = CompressorVisualizers::ID;
    visualizerManager.registerValueSupplier(VisualizerID::GainReduction, [this]() -> float {
        return compressor.getGainReduction();
    });


}

CompressorAudioProcessor::~CompressorAudioProcessor()
{
}

void CompressorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());
    // Prepare all DSP objects and buffers here
    compressor.prepare(numChannels, static_cast<size_t>(samplesPerBlock), static_cast<float>(sampleRate));

    
    // Initialize DSP with parameter defaults (defined in Params.h) (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void CompressorAudioProcessor::releaseResources()
{
    // Release DSP resources here
    compressor.reset();

    // Clear visualizer states
    visualizerManager.clearStates();

}

void CompressorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Get audio buffer info
    const int numInputChannels = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Early return if no audio to process
    if (numInputChannels == 0 || numOutputChannels == 0 || numSamples == 0)
    {
        visualizerManager.clearStates(); // Make the visualizers reset when there's no audio
        return;
    }

    // Update all parameters from FIFO (GUI thread → Audio thread)
    parameterManager.update();

    // Update all visualizers (Audio thread → Visualizer states)
    visualizerManager.update();

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

    // Process your DSP here using the mapped buffer
    compressor.processBlock(
        buffer.getArrayOfReadPointers(), 
        buffer.getArrayOfWritePointers(), 
        static_cast<size_t>(numSamples));

}

void CompressorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // This is taken care of by the parameter manager automatically
    parameterManager.saveState(destData);
}

void CompressorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // This is taken care of by the parameter manager automatically
    parameterManager.loadState(data, sizeInBytes);
}

bool CompressorAudioProcessor::acceptsMidi() const
{
    return true;
}




//==============================================================================
const juce::String CompressorAudioProcessor::getName() const { return JucePlugin_Name; }
bool CompressorAudioProcessor::producesMidi() const { return false; }
bool CompressorAudioProcessor::isMidiEffect() const { return false; }
double CompressorAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int CompressorAudioProcessor::getNumPrograms() { return 1; }
int CompressorAudioProcessor::getCurrentProgram() { return 0; }
void CompressorAudioProcessor::setCurrentProgram(int) { }
const juce::String CompressorAudioProcessor::getProgramName(int) { return {}; }
void CompressorAudioProcessor::changeProgramName(int, const juce::String&) { }
bool CompressorAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* CompressorAudioProcessor::createEditor() { return new CompressorAudioProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CompressorAudioProcessor();
}