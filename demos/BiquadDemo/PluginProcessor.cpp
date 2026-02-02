#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>
#include <iostream>
#include <jonssonic/utils/buffer_utils.h>

BiquadDemoAudioProcessor::BiquadDemoAudioProcessor() : parameterManager(BiquadDemoParams().createParams(), *this) {
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
    using ID = BiquadDemoParams::ID;

    parameterManager.on(ID::Gain, [this](float value, bool skipSmoothing) {
        biquadFilter.setGain(jnsc::Gain<float>::Decibels(value));
        DBG("[DEBUG] Gain changed: " + juce::String(value));
    });

    parameterManager.on(ID::Frequency, [this](float value, bool skipSmoothing) {
        biquadFilter.setFreq(jnsc::Frequency<float>::Hertz(value));
        DBG("[DEBUG] Frequency changed: " + juce::String(value) + " Hz");
    });

    parameterManager.on(ID::Q, [this](float value, bool skipSmoothing) {
        biquadFilter.setQ(value);
        DBG("[DEBUG] Q changed: " + juce::String(value));
    });

    parameterManager.on(ID::FilterType, [this](int value, bool skipSmoothing) {
        biquadFilter.setType(static_cast<jnsc::BiquadType>(value));
        DBG("[DEBUG] Filter Type changed: " + juce::String(value));
    });
}

BiquadDemoAudioProcessor::~BiquadDemoAudioProcessor() {}

void BiquadDemoAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());
    // Prepare all DSP objects and buffers here
    biquadFilter.prepare(numChannels, static_cast<float>(sampleRate));

    // Initialize DSP with parameter defaults (defined in Params.h) (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void BiquadDemoAudioProcessor::releaseResources() {
    // Release DSP resources here
    biquadFilter.reset();
}

void BiquadDemoAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
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
    jnsc::utils::mapChannels<float>(buffer.getArrayOfReadPointers(),
                                    buffer.getArrayOfWritePointers(),
                                    numInputChannels,
                                    numOutputChannels,
                                    numSamples);

    // Process audio with biquad filter
    biquadFilter.processBlock(buffer.getArrayOfReadPointers(),  // dry input
                              buffer.getArrayOfWritePointers(), // output
                              static_cast<size_t>(numSamples)); // number of samples
}

void BiquadDemoAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // This is taken care of by the parameter manager automatically
    parameterManager.saveState(destData);
}

void BiquadDemoAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // This is taken care of by the parameter manager automatically
    parameterManager.loadState(data, sizeInBytes);
}

bool BiquadDemoAudioProcessor::acceptsMidi() const {
    return true;
}

//==============================================================================
const juce::String BiquadDemoAudioProcessor::getName() const {
    return JucePlugin_Name;
}
bool BiquadDemoAudioProcessor::producesMidi() const {
    return false;
}
bool BiquadDemoAudioProcessor::isMidiEffect() const {
    return false;
}
double BiquadDemoAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}
int BiquadDemoAudioProcessor::getNumPrograms() {
    return 1;
}
int BiquadDemoAudioProcessor::getCurrentProgram() {
    return 0;
}
void BiquadDemoAudioProcessor::setCurrentProgram(int) {}
const juce::String BiquadDemoAudioProcessor::getProgramName(int) {
    return {};
}
void BiquadDemoAudioProcessor::changeProgramName(int, const juce::String&) {}
bool BiquadDemoAudioProcessor::hasEditor() const {
    return true;
}
juce::AudioProcessorEditor* BiquadDemoAudioProcessor::createEditor() {
    return new BiquadDemoAudioProcessorEditor(*this);
}
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new BiquadDemoAudioProcessor();
}