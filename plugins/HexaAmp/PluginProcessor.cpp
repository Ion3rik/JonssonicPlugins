#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>

HexaAmpAudioProcessor::HexaAmpAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::mono(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameterManager(HexaAmpParams().createParams(), *this) {
    auto& apvts = parameterManager.getAPVTS();
    const auto bypassID = jnsc::juce_interface::idToString(HexaAmpParams::ID::Bypass);
    bypassParameter = apvts.getParameter(bypassID);

    // Register callbacks for parameter changes
    using ID = HexaAmpParams::ID;

    parameterManager.on(ID::Bypass, [this](float value, bool /*skipSmoothing*/) { bypassed = value >= 0.5f; });

    parameterManager.on(ID::Drive, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setDrive(value, skipSmoothing);
    });

    parameterManager.on(ID::Bite, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setBite(value, skipSmoothing);
    });

    parameterManager.on(ID::Squish, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setSquish(value, skipSmoothing);
    });

    parameterManager.on(ID::Level, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setLevel(value, skipSmoothing);
    });

    parameterManager.on(ID::Color, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setColor(value, skipSmoothing);
    });

    parameterManager.on(ID::Tilt, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setTilt(value, skipSmoothing);
    });
    parameterManager.on(ID::PreAmpCompressorAttack, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setPreAmpCompressorAttack(value, skipSmoothing);
    });
    parameterManager.on(ID::PreAmpCompressorRelease, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setPreAmpCompressorRelease(value, skipSmoothing);
    });
    parameterManager.on(ID::PowerAmpCompressorAttack, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setPowerAmpCompressorAttack(value, skipSmoothing);
    });
    parameterManager.on(ID::PowerAmpCompressorRelease, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setPowerAmpCompressorRelease(value, skipSmoothing);
    });
    parameterManager.on(ID::MidBoostFrequency, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setMidBoostFrequency(value, skipSmoothing);
    });
    parameterManager.on(ID::PowerAmpShape, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setPowerAmpShape(value, skipSmoothing);
    });
    parameterManager.on(ID::PreAmpCompressorThresholdMin, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setPreAmpCompressorThresholdMin(value, skipSmoothing);
    });
    parameterManager.on(ID::PreAmpCompressorThresholdMax, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setPreAmpCompressorThresholdMax(value, skipSmoothing);
    });
    parameterManager.on(ID::PowerAmpCompressorThresholdMin, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setPowerAmpCompressorThresholdMin(value, skipSmoothing);
    });
    parameterManager.on(ID::PowerAmpCompressorThresholdMax, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setPowerAmpCompressorThresholdMax(value, skipSmoothing);
    });
    parameterManager.on(ID::Makeup, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaAmp.setMakeup(value, skipSmoothing);
    });
}

HexaAmpAudioProcessor::~HexaAmpAudioProcessor() {}

bool HexaAmpAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    if (layouts.inputBuses.size() != 1 || layouts.outputBuses.size() != 1)
        return false;

    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono() &&
           (layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono() ||
            layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo());
}

void HexaAmpAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    effectPrepared = false;
    // Prepare all DSP objects and buffers here
    hexaAmp.prepare(static_cast<size_t>(samplesPerBlock), static_cast<float>(sampleRate));
    setLatencySamples(static_cast<int>(hexaAmp.getLatencySamples()));
    effectPrepared = true;

    // Initialize DSP with current parameter values (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void HexaAmpAudioProcessor::releaseResources() {
    // Release DSP resources here
    effectPrepared = false;
    hexaAmp.reset();
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
    if (bypassed) {
        for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
            buffer.clear(channel, 0, numSamples);
        return;
    }

    // Handle denormals
    juce::ScopedNoDenormals noDenormals;

    if (!effectPrepared)
        return;

    hexaAmp.processBlock(buffer.getReadPointer(0), buffer.getWritePointer(0), static_cast<size_t>(numSamples));
    for (int channel = 1; channel < numOutputChannels; ++channel)
        std::copy_n(buffer.getReadPointer(0), numSamples, buffer.getWritePointer(channel));
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
