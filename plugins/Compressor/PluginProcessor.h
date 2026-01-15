#pragma once

#include "Params.h"
#include "Visualizers.h"
#include <MinimalJuceHeader.h>
#include <jonssonic/effects/compressor.h>
#include <parameters/ParameterManager.h>
#include <visualizers/VisualizerManager.h>

class CompressorAudioProcessor : public juce::AudioProcessor {
  public:
    //==============================================================================
    CompressorAudioProcessor();
    ~CompressorAudioProcessor() override;

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

    // Visualizer manager access for editor
    jnsc::juce_interface::VisualizerManager<CompressorVisualizers::ID>& getVisualizerManager() {
        return visualizerManager;
    }

  private:
    // Bus Properties
    static constexpr int mainInputBus = 0;
    static constexpr int sidechainBus = 1;

    // DSP objects and buffers
    jnsc::effects::Compressor<float> compressor; // Compressor DSP object

    // Parameter manager
    jnsc::juce_interface::ParameterManager<CompressorParams::ID> parameterManager;

    // Visualizer manager
    jnsc::juce_interface::VisualizerManager<CompressorVisualizers::ID> visualizerManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorAudioProcessor)
};