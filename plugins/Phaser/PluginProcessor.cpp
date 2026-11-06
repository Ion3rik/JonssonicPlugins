#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>
#include <iostream>
#include <jonssonic/utils/buffer_utils.h>

PhaserAudioProcessor::PhaserAudioProcessor() : parameterManager(PhaserParams().createParams(), *this) {
    // Print all APVTS parameter IDs at startup
    auto& apvts = parameterManager.getAPVTS();
    DBG("[DEBUG] APVTS parameter IDs at startup:");
    for (auto* param : apvts.processor.getParameters()) {
        if (auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(param)) {
            DBG("  " + paramWithID->paramID);
        }
    }

    // ...existing code...

    // Register callbacks for parameter changes
    using ID = PhaserParams::ID;

    parameterManager.on(ID::Rate, [this](float value, bool skipSmoothing) {
        DBG("[DSP] Rate changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        phaser.setRate(value, skipSmoothing);
    });

    parameterManager.on(ID::Depth, [this](float value, bool skipSmoothing) {
        DBG("[DSP] Depth changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        phaser.setDepth(value * 0.01f, skipSmoothing);
    });

    parameterManager.on(ID::Spread, [this](float value, bool skipSmoothing) {
        DBG("[DSP] Spread changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        phaser.setSpread(value * 0.01f, skipSmoothing);
    });

    parameterManager.on(ID::CenterFrequency, [this](float value, bool skipSmoothing) {
        DBG("[DSP] Center frequency changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        phaser.setCenterFrequency(value, skipSmoothing);
    });

    parameterManager.on(ID::Feedback, [this](float value, bool skipSmoothing) {
        DBG("[DSP] Feedback changed: " + juce::String(value) +
            ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        phaser.setFeedback(value * 0.01f, skipSmoothing);
    });

    parameterManager.on(ID::Mix, [this](float value, bool skipSmoothing) {
        DBG("[DSP] Mix changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        dryWetMixer.setMix(value * 0.01f, skipSmoothing);
    });
}

PhaserAudioProcessor::~PhaserAudioProcessor() {}

void PhaserAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());
    phaser.prepare(numChannels, sampleRate);
    fxBuffer.resize(numChannels, samplesPerBlock);
    dryWetMixer.prepare(numChannels, sampleRate);
    dryWetMixer.setControlSmoothingTime(jnsc::Time<float>::Milliseconds(50.0f));

    // Initialize DSP with parameter defaults (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void PhaserAudioProcessor::releaseResources() {
    phaser.reset();
    dryWetMixer.reset();
    fxBuffer.resize(0, 0); // Free buffer memory
}

void PhaserAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
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
        fxBuffer.writePtrs(),            // jnsc::AudioBuffer<float> uses writePtrs() for non-const access
        numInputChannels,
        numOutputChannels,
        numSamples);

    // Process wet signal in fxBuffer
    phaser.processBlock(fxBuffer.readPtrs(), fxBuffer.writePtrs(), static_cast<size_t>(numSamples));

    // Mix wet signal with dry (addFrom adds wet to existing dry signal)
    dryWetMixer.processBlock(buffer.getArrayOfReadPointers(),  // dry buffer
                             fxBuffer.readPtrs(),              // wet buffer
                             buffer.getArrayOfWritePointers(), // final output
                             static_cast<size_t>(numSamples)); // number of samples
}

void PhaserAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    parameterManager.saveState(destData);
}

void PhaserAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    parameterManager.loadState(data, sizeInBytes);
}

bool PhaserAudioProcessor::acceptsMidi() const {
    return true;
}

//==============================================================================
const juce::String PhaserAudioProcessor::getName() const {
    return JucePlugin_Name;
}
bool PhaserAudioProcessor::producesMidi() const {
    return false;
}
bool PhaserAudioProcessor::isMidiEffect() const {
    return false;
}
double PhaserAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}
int PhaserAudioProcessor::getNumPrograms() {
    return 1;
}
int PhaserAudioProcessor::getCurrentProgram() {
    return 0;
}
void PhaserAudioProcessor::setCurrentProgram(int) {}
const juce::String PhaserAudioProcessor::getProgramName(int) {
    return {};
}
void PhaserAudioProcessor::changeProgramName(int, const juce::String&) {}
bool PhaserAudioProcessor::hasEditor() const {
    return true;
}
juce::AudioProcessorEditor* PhaserAudioProcessor::createEditor() {
    return new PhaserAudioProcessorEditor(*this);
}
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new PhaserAudioProcessor();
}