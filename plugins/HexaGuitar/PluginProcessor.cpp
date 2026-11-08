#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>

HexaGuitarAudioProcessor::HexaGuitarAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::mono(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameterManager(HexaGuitarParams().createParams(), *this) {
    auto& apvts = parameterManager.getAPVTS();
    const auto bypassID = jnsc::juce_interface::idToString(HexaGuitarParams::ID::Bypass);
    bypassParameter = apvts.getParameter(bypassID);

    // Register callbacks for parameter changes
    using ID = HexaGuitarParams::ID;

    parameterManager.on(ID::Bypass, [this](float value, bool /*skipSmoothing*/) { bypassed = value >= 0.5f; });

    parameterManager.on(ID::Drive, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setDrive(value, skipSmoothing);
    });

    parameterManager.on(ID::Bite, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setBite(value, skipSmoothing);
    });

    parameterManager.on(ID::Squish, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setSquish(value, skipSmoothing);
    });

    parameterManager.on(ID::Level, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setLevel(value, skipSmoothing);
    });

    parameterManager.on(ID::Color, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setColor(value, skipSmoothing);
    });

    parameterManager.on(ID::Tilt, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setTilt(value, skipSmoothing);
    });
    parameterManager.on(ID::PreAmpCompressorAttack, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setPreAmpCompressorAttack(value, skipSmoothing);
    });
    parameterManager.on(ID::PreAmpCompressorRelease, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setPreAmpCompressorRelease(value, skipSmoothing);
    });
    parameterManager.on(ID::PowerAmpCompressorAttack, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setPowerAmpCompressorAttack(value, skipSmoothing);
    });
    parameterManager.on(ID::PowerAmpCompressorRelease, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setPowerAmpCompressorRelease(value, skipSmoothing);
    });
    parameterManager.on(ID::MidBoostFrequency, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setMidBoostFrequency(value, skipSmoothing);
    });
    parameterManager.on(ID::PowerAmpShape, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setPowerAmpShape(value, skipSmoothing);
    });
    parameterManager.on(ID::PreAmpCompressorThresholdMin, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setPreAmpCompressorThresholdMin(value, skipSmoothing);
    });
    parameterManager.on(ID::PreAmpCompressorThresholdMax, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setPreAmpCompressorThresholdMax(value, skipSmoothing);
    });
    parameterManager.on(ID::PowerAmpCompressorThresholdMin, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setPowerAmpCompressorThresholdMin(value, skipSmoothing);
    });
    parameterManager.on(ID::PowerAmpCompressorThresholdMax, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setPowerAmpCompressorThresholdMax(value, skipSmoothing);
    });
    parameterManager.on(ID::Makeup, [this](float value, bool skipSmoothing) {
        if (effectPrepared)
            hexaGuitar.setMakeup(value, skipSmoothing);
    });
}

HexaGuitarAudioProcessor::~HexaGuitarAudioProcessor() {}

bool HexaGuitarAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    if (layouts.inputBuses.size() != 1 || layouts.outputBuses.size() != 1)
        return false;

    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono() &&
           (layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono() ||
            layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo());
}

void HexaGuitarAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    effectPrepared = false;
    // Prepare all DSP objects and buffers here
    hexaGuitar.prepare(static_cast<size_t>(samplesPerBlock), static_cast<float>(sampleRate));
    setLatencySamples(static_cast<int>(hexaGuitar.getLatencySamples()));
    effectPrepared = true;

    // Initialize DSP with current parameter values (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void HexaGuitarAudioProcessor::releaseResources() {
    // Release DSP resources here
    effectPrepared = false;
    hexaGuitar.reset();
}

void HexaGuitarAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
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

    hexaGuitar.processBlock(buffer.getReadPointer(0), buffer.getWritePointer(0), static_cast<size_t>(numSamples));
    for (int channel = 1; channel < numOutputChannels; ++channel)
        std::copy_n(buffer.getReadPointer(0), numSamples, buffer.getWritePointer(channel));
}

void HexaGuitarAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // This is taken care of by the parameter manager automatically
    parameterManager.saveState(destData);
}

void HexaGuitarAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // This is taken care of by the parameter manager automatically
    parameterManager.loadState(data, sizeInBytes);
}

bool HexaGuitarAudioProcessor::acceptsMidi() const {
    return true;
}

//==============================================================================
const juce::String HexaGuitarAudioProcessor::getName() const {
    return JucePlugin_Name;
}
bool HexaGuitarAudioProcessor::producesMidi() const {
    return false;
}
bool HexaGuitarAudioProcessor::isMidiEffect() const {
    return false;
}
double HexaGuitarAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}
int HexaGuitarAudioProcessor::getNumPrograms() {
    return 1;
}
int HexaGuitarAudioProcessor::getCurrentProgram() {
    return 0;
}
void HexaGuitarAudioProcessor::setCurrentProgram(int) {}
const juce::String HexaGuitarAudioProcessor::getProgramName(int) {
    return {};
}
void HexaGuitarAudioProcessor::changeProgramName(int, const juce::String&) {}
bool HexaGuitarAudioProcessor::hasEditor() const {
    return true;
}
juce::AudioProcessorEditor* HexaGuitarAudioProcessor::createEditor() {
    return new HexaGuitarAudioProcessorEditor(*this);
}
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new HexaGuitarAudioProcessor();
}
