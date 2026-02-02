#pragma once
#include "Params.h"
#include <MinimalJuceHeader.h>
#include <jonssonic/core/nonlinear/wave_shaper_processor.h>
#include <jonssonic/core/oversampling/oversampled_processor.h>
#include <parameters/ParameterManager.h>

class OversamplingDemoAudioProcessor : public juce::AudioProcessor {
  public:
    OversamplingDemoAudioProcessor();
    ~OversamplingDemoAudioProcessor() override;

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
    jnsc::OversampledProcessor<float> oversampledProcessor;
    jnsc::WaveShaperProcessor<float, jnsc::WaveShaperType::HardClip> distortion;

    int currentOversamplingFactor = 1; // 1=None, 2=2x, 4=4x, 8=8x, 16=16x

    // Parameter manager
    jnsc::juce_interface::ParameterManager<OversamplingDemoParams::ID> parameterManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OversamplingDemoAudioProcessor)
};