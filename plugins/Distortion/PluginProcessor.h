#pragma once
#include <JuceHeader.h>
#include <Jonssonic/core/mixing/DryWetMixer.h>
#include <parameters/ParameterManager.h>
#include "Params.h"


class DistortionAudioProcessor : public juce::AudioProcessor
{
public:
    DistortionAudioProcessor();
    ~DistortionAudioProcessor() override;

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
    // DSP objects and buffers
    juce::AudioBuffer<float> fxBuffer; // Buffer for effect processing
    Jonssonic::DryWetMixer<float> dryWetMixer; // Dry/wet mixer
    
    // Parameter manager
    Jonssonic::ParameterManager<DistortionParams::ID> parameterManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionAudioProcessor)
};