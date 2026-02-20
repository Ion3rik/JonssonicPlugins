#pragma once

#include "Params.h"
#include <MinimalJuceHeader.h>
#include <jonssonic/core/filters/state_variable_filter.h>
#include <parameters/ParameterManager.h>

class SVFDemoAudioProcessor : public juce::AudioProcessor {
  public:
    SVFDemoAudioProcessor();
    ~SVFDemoAudioProcessor() override;

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
    jnsc::StateVariableFilter<float> svf;

    // Parameter manager
    jnsc::juce_interface::ParameterManager<SVFDemoParams::ID> parameterManager;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SVFDemoAudioProcessor)
};