#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <jonssonic/utils/buffer_utils.h>

HexaAmpAudioProcessor::HexaAmpAudioProcessor() : parameterManager(HexaAmpParams().createParams(), *this) {
    // ============================================================================
    // [DEBUG]: Prints all APVTS parameter IDs at startup
    // This helps verify that your parameters are registered correctly.
    // You can remove or comment out this block when you no longer need it.
    auto& apvts = parameterManager.getAPVTS();
    const auto bypassID = jnsc::juce_interface::idToString(HexaAmpParams::ID::Bypass);
    bypassParameter = apvts.getParameter(bypassID);
    bypassValue = apvts.getRawParameterValue(bypassID);
    DBG("[DEBUG] APVTS parameter IDs at startup:");
    for (auto* param : apvts.processor.getParameters()) {
        if (auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(param)) {
            DBG("  " + paramWithID->paramID);
        }
    }
    // ============================================================================

    // Register callbacks for parameter changes
    using ID = HexaAmpParams::ID;

    parameterManager.on(ID::Drive, [](float /*value*/, bool /*skipSmoothing*/) {
        // Call your DSP drive setter here
    });

    parameterManager.on(ID::Bite, [](float /*value*/, bool /*skipSmoothing*/) {
        // Call your DSP bite setter here
    });

    parameterManager.on(ID::Squish, [](float /*value*/, bool /*skipSmoothing*/) {
        // Call your DSP squish setter here
    });

    parameterManager.on(ID::Level, [](float /*value*/, bool /*skipSmoothing*/) {
        // Call your DSP level setter here
    });

    parameterManager.on(ID::Color, [](float /*value*/, bool /*skipSmoothing*/) {
        // Call your DSP color setter here
    });

    parameterManager.on(ID::Body, [](float /*value*/, bool /*skipSmoothing*/) {
        // Call your DSP body setter here
    });
}

HexaAmpAudioProcessor::~HexaAmpAudioProcessor() {}

void HexaAmpAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());
    // Prepare all DSP objects and buffers here
    dryWetMixer.prepare(numChannels, static_cast<float>(sampleRate));
    fxBuffer.resize(numChannels, static_cast<size_t>(samplesPerBlock));

    // Initialize DSP with parameter defaults (defined in Params.h) (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void HexaAmpAudioProcessor::releaseResources() {
    // Release DSP resources here
    dryWetMixer.reset();
    fxBuffer.resize(0, 0);
}

void HexaAmpAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    // Get audio buffer info
    const int numInputChannels = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // Early return if no audio to process
    if (numInputChannels == 0 || numOutputChannels == 0 || numSamples == 0)
        return;
    // Update all parameters from FIFO (GUI thread → Audio thread)
    parameterManager.update();

    // Keep input audio untouched when bypassed, and skip every DSP stage.
    if (bypassValue->load(std::memory_order_relaxed) >= 0.5f) {
        for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
            buffer.clear(channel, 0, numSamples);
        return;
    }

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

    // DSP processing here (this template includes only a dry/wet mixer as an example)
    dryWetMixer.processBlock(buffer.getArrayOfReadPointers(),  // dry buffer
                             fxBuffer.readPtrs(),              // wet buffer
                             buffer.getArrayOfWritePointers(), // final output
                             static_cast<size_t>(numSamples)); // number of samples
}

void HexaAmpAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // This is taken care of by the parameter manager automatically
    parameterManager.saveState(destData);
}

void HexaAmpAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // This is taken care of by the parameter manager automatically
    parameterManager.loadState(data, sizeInBytes);
}

bool HexaAmpAudioProcessor::acceptsMidi() const {
    return true;
}

//==============================================================================
const juce::String HexaAmpAudioProcessor::getName() const {
    return JucePlugin_Name;
}
bool HexaAmpAudioProcessor::producesMidi() const {
    return false;
}
bool HexaAmpAudioProcessor::isMidiEffect() const {
    return false;
}
double HexaAmpAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}
int HexaAmpAudioProcessor::getNumPrograms() {
    return 1;
}
int HexaAmpAudioProcessor::getCurrentProgram() {
    return 0;
}
void HexaAmpAudioProcessor::setCurrentProgram(int) {}
const juce::String HexaAmpAudioProcessor::getProgramName(int) {
    return {};
}
void HexaAmpAudioProcessor::changeProgramName(int, const juce::String&) {}
bool HexaAmpAudioProcessor::hasEditor() const {
    return true;
}
juce::AudioProcessorEditor* HexaAmpAudioProcessor::createEditor() {
    return new HexaAmpAudioProcessorEditor(*this);
}
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new HexaAmpAudioProcessor();
}
