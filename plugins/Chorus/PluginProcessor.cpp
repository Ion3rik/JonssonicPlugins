#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>
#include <iostream>
#include <jonssonic/utils/buffer_utils.h>

ChorusAudioProcessor::ChorusAudioProcessor() : parameterManager(ChorusParams().createParams(), *this) {
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
    using ID = ChorusParams::ID;

    parameterManager.on(ID::Rate, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Rate changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        chorus.setRate(value, skipSmoothing);
    });

    parameterManager.on(ID::Depth, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Depth changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        chorus.setDepth(value * 0.01f, skipSmoothing); // We are converting from [0,100] to [0,1]
    });

    parameterManager.on(ID::Spread, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Spread changed: " + juce::String(value) +
            ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        chorus.setSpread(value * 0.01f, skipSmoothing); // We are converting from [0,100] to [0,1]
    });

    parameterManager.on(ID::Delay, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Delay changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        chorus.setDelayMs(value, skipSmoothing);
    });

    parameterManager.on(ID::Feedback, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Feedback changed: " + juce::String(value) +
            ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        chorus.setFeedback(value * 0.01f, skipSmoothing); // We are converting from [0,100] to [0,1]
    });

    parameterManager.on(ID::Mix, [this](float value, bool skipSmoothing) {
        DBG("[DEBUG] Mix changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        dryWetMixer.setMix(value * 0.01f); // We are converting from [0,100] to [0,1]
    });
}

ChorusAudioProcessor::~ChorusAudioProcessor() {}

void ChorusAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());
    // Prepare all DSP objects and buffers here
    dryWetMixer.prepare(numChannels, static_cast<float>(sampleRate));
    fxBuffer.resize(numChannels, static_cast<size_t>(samplesPerBlock));
    chorus.prepare(numChannels, static_cast<float>(sampleRate));

    // Initialize DSP with parameter defaults (defined in Params.h) (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void ChorusAudioProcessor::releaseResources() {
    // Release DSP resources here
    dryWetMixer.reset();
    fxBuffer.resize(0, 0);
    chorus.reset();
}

void ChorusAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
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
    jnsc::utils::mapChannels<float>(
        buffer.getArrayOfReadPointers(), // juce::AudioBuffer<float> uses getArrayOfReadPointers() for const access
        fxBuffer.writePtrs(),            // Jonssonic::AudioBuffer<float> uses writePtrs() for non-const access
        numInputChannels,
        numOutputChannels,
        numSamples);

    // Process the chorus effect
    chorus.processBlock(fxBuffer.readPtrs(), fxBuffer.writePtrs(), static_cast<size_t>(numSamples));

    // Dry/wet processing
    dryWetMixer.processBlock(buffer.getArrayOfReadPointers(),  // dry buffer
                             fxBuffer.readPtrs(),              // wet buffer
                             buffer.getArrayOfWritePointers(), // final output
                             static_cast<size_t>(numSamples)); // number of samples
}

void ChorusAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // This is taken care of by the parameter manager automatically
    parameterManager.saveState(destData);
}

void ChorusAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // This is taken care of by the parameter manager automatically
    parameterManager.loadState(data, sizeInBytes);
}

bool ChorusAudioProcessor::acceptsMidi() const {
    return true;
}

//==============================================================================
const juce::String ChorusAudioProcessor::getName() const {
    return JucePlugin_Name;
}
bool ChorusAudioProcessor::producesMidi() const {
    return false;
}
bool ChorusAudioProcessor::isMidiEffect() const {
    return false;
}
double ChorusAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}
int ChorusAudioProcessor::getNumPrograms() {
    return 1;
}
int ChorusAudioProcessor::getCurrentProgram() {
    return 0;
}
void ChorusAudioProcessor::setCurrentProgram(int) {}
const juce::String ChorusAudioProcessor::getProgramName(int) {
    return {};
}
void ChorusAudioProcessor::changeProgramName(int, const juce::String&) {}
bool ChorusAudioProcessor::hasEditor() const {
    return true;
}
juce::AudioProcessorEditor* ChorusAudioProcessor::createEditor() {
    return new ChorusAudioProcessorEditor(*this);
}
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new ChorusAudioProcessor();
}