#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <jonssonic/utils/buffer_utils.h>

SVFDemoAudioProcessor::SVFDemoAudioProcessor() : parameterManager(SVFDemoParams().createParams(), *this) {
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
    using ID = SVFDemoParams::ID;
    using ResponseType = jnsc::StateVariableFilter<float>::Response;

    parameterManager.on(ID::Frequency, [this](float newValue, bool skipSmoothing) {
        // Update the filter frequency immediately (no smoothing for demo purposes)
        svf.setFrequency(jnsc::Frequency<float>::Hertz(newValue));
    });

    parameterManager.on(ID::Q, [this](float newValue, bool skipSmoothing) {
        // Update the filter Q immediately (no smoothing for demo purposes)
        svf.setQ(newValue);
    });

    parameterManager.on(ID::Response, [this](int newValue, bool skipSmoothing) {
        // Update the filter type immediately based on the selected response
        ResponseType response = static_cast<ResponseType>(newValue);
        svf.setResponse(response);
    });
}

SVFDemoAudioProcessor::~SVFDemoAudioProcessor() {}

void SVFDemoAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());
    // Prepare all DSP objects and buffers here
    svf.prepare(numChannels, static_cast<float>(sampleRate));

    // Initialize DSP with parameter defaults (defined in Params.h) (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void SVFDemoAudioProcessor::releaseResources() {
    // Release DSP resources here
    svf.reset();
}

void SVFDemoAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
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

    // Process the audio through the state variable filter
    svf.processBlock(buffer.getArrayOfReadPointers(),
                     buffer.getArrayOfWritePointers(),
                     static_cast<size_t>(numSamples));
}

void SVFDemoAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // This is taken care of by the parameter manager automatically
    parameterManager.saveState(destData);
}

void SVFDemoAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // This is taken care of by the parameter manager automatically
    parameterManager.loadState(data, sizeInBytes);
}

bool SVFDemoAudioProcessor::acceptsMidi() const {
    return true;
}

//==============================================================================
const juce::String SVFDemoAudioProcessor::getName() const {
    return JucePlugin_Name;
}
bool SVFDemoAudioProcessor::producesMidi() const {
    return false;
}
bool SVFDemoAudioProcessor::isMidiEffect() const {
    return false;
}
double SVFDemoAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}
int SVFDemoAudioProcessor::getNumPrograms() {
    return 1;
}
int SVFDemoAudioProcessor::getCurrentProgram() {
    return 0;
}
void SVFDemoAudioProcessor::setCurrentProgram(int) {}
const juce::String SVFDemoAudioProcessor::getProgramName(int) {
    return {};
}
void SVFDemoAudioProcessor::changeProgramName(int, const juce::String&) {}
bool SVFDemoAudioProcessor::hasEditor() const {
    return true;
}
juce::AudioProcessorEditor* SVFDemoAudioProcessor::createEditor() {
    return new SVFDemoAudioProcessorEditor(*this);
}
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new SVFDemoAudioProcessor();
}