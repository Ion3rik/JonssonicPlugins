#include <iostream>
#include "PluginProcessor.h"
#include <JuceHeader.h>
#include "PluginEditor.h"
#include <Jonssonic/utils/BufferUtils.h>


FlangerAudioProcessor::FlangerAudioProcessor()
    : parameterManager(FlangerParams::createParams(), *this)
{
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
    using ID = FlangerParams::ID;
    
    parameterManager.on(ID::Rate, [this](float value, bool skipSmoothing) {
        DBG("[DSP] Rate changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        flanger.setRate(value, skipSmoothing);
    });

    parameterManager.on(ID::Depth, [this](float value, bool skipSmoothing) {
        DBG("[DSP] Depth changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        flanger.setDepth(value*0.01f, skipSmoothing);
    });

    parameterManager.on(ID::Spread, [this](float value, bool skipSmoothing) {
        DBG("[DSP] Spread changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        flanger.setSpread(value*0.01f, skipSmoothing);
    });

    parameterManager.on(ID::Delay, [this](float value, bool skipSmoothing) {
        DBG("[DSP] Delay changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        flanger.setDelayMs(value, skipSmoothing);
    });

    parameterManager.on(ID::Feedback, [this](float value, bool skipSmoothing) {
        DBG("[DSP] Feedback changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        flanger.setFeedback(value*0.01f, skipSmoothing);
    });

    parameterManager.on(ID::Mix, [this](float value, bool skipSmoothing) {
        DBG("[DSP] Mix changed: " + juce::String(value) + ", skipSmoothing: " + (skipSmoothing ? "true" : "false"));
        dryWetMixer.setMix(value*0.01f, skipSmoothing);
    });
}

FlangerAudioProcessor::~FlangerAudioProcessor()
{
}

void FlangerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());
    flanger.prepare(numChannels, sampleRate);
    fxBuffer.setSize(numChannels, samplesPerBlock);
    dryWetMixer.prepare(numChannels, sampleRate);
    
    // Initialize DSP with parameter defaults (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void FlangerAudioProcessor::releaseResources()
{
    flanger.reset();
    dryWetMixer.reset();
}

void FlangerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
        numInputChannels, 
        numOutputChannels, 
        numSamples);
        
    // Process wet signal in fxBuffer
    flanger.processBlock(fxBuffer.getArrayOfReadPointers(),
                         fxBuffer.getArrayOfWritePointers(),
                         static_cast<size_t>(numSamples));
    
    // Mix wet signal with dry (addFrom adds wet to existing dry signal)
    dryWetMixer.processBlock(buffer.getArrayOfReadPointers(),   // dry input
                             fxBuffer.getArrayOfReadPointers(), // wet input
                             buffer.getArrayOfWritePointers(),  // output
                             static_cast<size_t>(numSamples));  // number of samples
}

void FlangerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    parameterManager.saveState(destData);
}

void FlangerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    parameterManager.loadState(data, sizeInBytes);
}

bool FlangerAudioProcessor::acceptsMidi() const
{
    return true;
}




//==============================================================================
const juce::String FlangerAudioProcessor::getName() const { return JucePlugin_Name; }
bool FlangerAudioProcessor::producesMidi() const { return false; }
bool FlangerAudioProcessor::isMidiEffect() const { return false; }
double FlangerAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int FlangerAudioProcessor::getNumPrograms() { return 1; }
int FlangerAudioProcessor::getCurrentProgram() { return 0; }
void FlangerAudioProcessor::setCurrentProgram(int) { }
const juce::String FlangerAudioProcessor::getProgramName(int) { return {}; }
void FlangerAudioProcessor::changeProgramName(int, const juce::String&) { }
bool FlangerAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* FlangerAudioProcessor::createEditor() { return new FlangerAudioProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FlangerAudioProcessor();
}