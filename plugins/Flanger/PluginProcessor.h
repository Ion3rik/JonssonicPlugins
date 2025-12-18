#pragma once
#include <JuceHeader.h>
#include <Jonssonic/core/common/AudioBuffer.h>
#include <Jonssonic/effects/Flanger.h>
#include <Jonssonic/core/mixing/DryWetMixer.h>
#include <parameters/ParameterManager.h>
#include "Params.h"


class FlangerAudioProcessor : public juce::AudioProcessor
{
public:
    FlangerAudioProcessor();
    ~FlangerAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void releaseResources() override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;


    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;
    //==============================================================================
    
    // Parameter access for editor
    juce::AudioProcessorValueTreeState& getAPVTS() { return parameterManager.getAPVTS(); }

private:
    // DSP objects
    Jonssonic::Flanger<float> flanger;
    Jonssonic::AudioBuffer<float> fxBuffer;
    Jonssonic::DryWetMixer<float> dryWetMixer;
    
    // Parameter management
    Jonssonic::ParameterManager<FlangerParams::ID> parameterManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlangerAudioProcessor)
};