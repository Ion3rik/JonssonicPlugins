#include <iostream>
#include "PluginProcessor.h"
#include <JuceHeader.h>
#include "PluginEditor.h"
#include <Jonssonic/utils/BufferUtils.h>


DelayAudioProcessor::DelayAudioProcessor()
    : parameterManager(DelayParams::createParams(), *this)
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
    using ID = DelayParams::ID;
    
    parameterManager.on(ID::Mix, [this](float value, bool skipSmoothing) {
        dryWetMixer.setMix(value * 0.01f);
        
    });

    parameterManager.on(ID::DelayTimeMs, [this](float value, bool skipSmoothing) {
        delayEffect.setDelayMs(value, skipSmoothing);
    });

    parameterManager.on(ID::Feedback, [this](float value, bool skipSmoothing) {
        delayEffect.setFeedback(value * 0.01f, skipSmoothing);
    });

    parameterManager.on(ID::Damping, [this](float value, bool skipSmoothing) {
        delayEffect.setDamping(value * 0.01f, skipSmoothing);
    });

    parameterManager.on(ID::PingPong, [this](float value, bool skipSmoothing) {
        delayEffect.setPingPong(value * 0.01f, skipSmoothing);
    });

    parameterManager.on(ID::ModDepth, [this](float value, bool skipSmoothing) {
        delayEffect.setModDepth(value * 0.01f, skipSmoothing);
    });


}

DelayAudioProcessor::~DelayAudioProcessor()
{
}

void DelayAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    auto numChannels = static_cast<size_t>(getTotalNumOutputChannels());

    // Prepare all DSP objects and buffers here
    dryWetMixer.prepare(numChannels, static_cast<float>(sampleRate));
    fxBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    delayEffect.prepare(numChannels, static_cast<float>(sampleRate), 2010.0f); // Max delay 2010 ms
    
    // Initialize DSP with parameter defaults (defined in Params.h) (skip smoothing for instant setup)
    parameterManager.syncAll(true);
}

void DelayAudioProcessor::releaseResources()
{
    // Release DSP resources here
    dryWetMixer.reset();
    fxBuffer.setSize(0, 0);
    delayEffect.reset();
}

void DelayAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
        fxBuffer.getArrayOfWritePointers(), 
        numInputChannels, 
        numOutputChannels, 
        numSamples);

    
    // DSP processing here (this template includes only a dry/wet mixer as an example)
    delayEffect.processBlock(fxBuffer.getArrayOfWritePointers(),
                            fxBuffer.getArrayOfWritePointers(),
                             static_cast<size_t>(numSamples));  
    dryWetMixer.processBlock(buffer.getArrayOfReadPointers(),   // dry input
                             fxBuffer.getArrayOfReadPointers(), // wet input
                             buffer.getArrayOfWritePointers(),  // output
                             static_cast<size_t>(numSamples));  // number of samples
}

void DelayAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // This is taken care of by the parameter manager automatically
    parameterManager.saveState(destData);
}

void DelayAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // This is taken care of by the parameter manager automatically
    parameterManager.loadState(data, sizeInBytes);
}

bool DelayAudioProcessor::acceptsMidi() const
{
    return true;
}




//==============================================================================
const juce::String DelayAudioProcessor::getName() const { return JucePlugin_Name; }
bool DelayAudioProcessor::producesMidi() const { return false; }
bool DelayAudioProcessor::isMidiEffect() const { return false; }
double DelayAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int DelayAudioProcessor::getNumPrograms() { return 1; }
int DelayAudioProcessor::getCurrentProgram() { return 0; }
void DelayAudioProcessor::setCurrentProgram(int) { }
const juce::String DelayAudioProcessor::getProgramName(int) { return {}; }
void DelayAudioProcessor::changeProgramName(int, const juce::String&) { }
bool DelayAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* DelayAudioProcessor::createEditor() { return new DelayAudioProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}