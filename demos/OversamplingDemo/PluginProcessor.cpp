#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>
#include <iostream>

OversamplingDemoAudioProcessor::OversamplingDemoAudioProcessor()
    : parameterManager(OversamplingDemoParams().createParams(), *this) {
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
    using ID = OversamplingDemoParams::ID;

    parameterManager.on(ID::Drive, [this](float value, bool skipSmoothing) {
        distortion.setInputGain(jnsc::Gain<float>::Decibels(value));
        DBG("[DEBUG] Drive changed: " + juce::String(value) + " dB");
    });

    parameterManager.on(ID::OutputGain, [this](float value, bool skipSmoothing) {
        distortion.setOutputGain(jnsc::Gain<float>::Decibels(value));
        DBG("[DEBUG] Output Gain changed: " + juce::String(value) + " dB");
    });

    // Map parameter index to actual oversampling factor
    parameterManager.on(ID::OversamplingFactor, [this](int value, bool skipSmoothing) {
        const int factors[] = {1, 2, 4, 8, 16};
        currentOversamplingFactor = factors[value];
        DBG("[DEBUG] Oversampling Factor changed: " + juce::String(currentOversamplingFactor) + "x");
    });
}

OversamplingDemoAudioProcessor::~OversamplingDemoAudioProcessor() {}

void OversamplingDemoAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());

    // Prepare oversampled processor wrapper
    oversampledProcessor.prepare(numChannels, static_cast<size_t>(samplesPerBlock));

    // Prepare distortion stage
    distortion.prepare(numChannels, static_cast<float>(sampleRate));

    // Initialize DSP with parameter defaults (defined in Params.h) (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void OversamplingDemoAudioProcessor::releaseResources() {
    // Release DSP resources here
    oversampledProcessor.reset();
}

void OversamplingDemoAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    // Get audio buffer info
    const int numChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // Early return if no audio to process
    if (numChannels == 0 || numSamples == 0)
        return;

    // Update all parameters from FIFO (GUI thread → Audio thread)
    parameterManager.update();

    // Handle denormals
    juce::ScopedNoDenormals noDenormals;

    // Process with current oversampling factor
    oversampledProcessor.processBlock(currentOversamplingFactor,
                                      buffer.getArrayOfReadPointers(),
                                      buffer.getArrayOfWritePointers(),
                                      static_cast<size_t>(numSamples),
                                      [this](const float* const* input, float* const* output, size_t samples) {
                                          distortion.processBlock(input, output, samples);
                                      });
}

void OversamplingDemoAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // This is taken care of by the parameter manager automatically
    parameterManager.saveState(destData);
}

void OversamplingDemoAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // This is taken care of by the parameter manager automatically
    parameterManager.loadState(data, sizeInBytes);
}

bool OversamplingDemoAudioProcessor::acceptsMidi() const {
    return true;
}

//==============================================================================
const juce::String OversamplingDemoAudioProcessor::getName() const {
    return JucePlugin_Name;
}
bool OversamplingDemoAudioProcessor::producesMidi() const {
    return false;
}
bool OversamplingDemoAudioProcessor::isMidiEffect() const {
    return false;
}
double OversamplingDemoAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}
int OversamplingDemoAudioProcessor::getNumPrograms() {
    return 1;
}
int OversamplingDemoAudioProcessor::getCurrentProgram() {
    return 0;
}
void OversamplingDemoAudioProcessor::setCurrentProgram(int) {}
const juce::String OversamplingDemoAudioProcessor::getProgramName(int) {
    return {};
}
void OversamplingDemoAudioProcessor::changeProgramName(int, const juce::String&) {}
bool OversamplingDemoAudioProcessor::hasEditor() const {
    return true;
}
juce::AudioProcessorEditor* OversamplingDemoAudioProcessor::createEditor() {
    return new OversamplingDemoAudioProcessorEditor(*this);
}
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new OversamplingDemoAudioProcessor();
}