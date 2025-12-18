#include <iostream>
#include "PluginProcessor.h"
#include <JuceHeader.h>
#include "PluginEditor.h"
#include <Jonssonic/utils/BufferUtils.h>
#include <Jonssonic/utils/MathUtils.h>


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
    
    parameterManager.on(ID::Drive, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Drive changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        distortion.setDriveDb(value, skipSmoothing);
    });

    parameterManager.on(ID::Asymmetry, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Asymmetry changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        distortion.setAsymmetry(value * 0.01f, skipSmoothing);
        
    });

    parameterManager.on(ID::Shape, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Shape changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        distortion.setShape(value * 0.01f, skipSmoothing);
    });

    parameterManager.on(ID::Tone, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Tone changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        distortion.setToneFrequency(value, skipSmoothing);
    });

    parameterManager.on(ID::Mix, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Mix changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        distortion.setMix(value * 0.01f, skipSmoothing);
    });

    parameterManager.on(ID::Output, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Output changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        distortion.setOutputGainDb(value, skipSmoothing);
    });

    parameterManager.on(ID::Oversampling, [this](float enabled, bool /*skipSmoothing*/) {
        bool isEnabled = (enabled >= 0.5f);
        DBG("[DEBUG] Oversampling changed: " + juce::String(isEnabled ? "true" : "false"));
        distortion.setOversamplingEnabled(isEnabled);
    });

}

DistortionAudioProcessor::~DistortionAudioProcessor()
{
}

void DistortionAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());
    // Prepare all DSP objects and buffers here
    distortion.prepare(numChannels, static_cast<size_t>(samplesPerBlock), static_cast<float>(sampleRate));
    fxBuffer.resize(numChannels, static_cast<size_t>(samplesPerBlock));
    
    // Initialize DSP with parameter defaults (defined in Params.h) (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void DistortionAudioProcessor::releaseResources()
{
    // Release DSP resources here
    distortion.reset();
    fxBuffer.clear();
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
        buffer.getArrayOfWritePointers(),  
        static_cast<size_t>(numInputChannels), 
        static_cast<size_t>(numOutputChannels), 
        static_cast<size_t>(numSamples));

    // Process distortion effect (dry/wet mixing and output gain applied inside)
    distortion.processBlock(buffer.getArrayOfReadPointers(),  
                            buffer.getArrayOfWritePointers(),  
                            static_cast<size_t>(numSamples));



    
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