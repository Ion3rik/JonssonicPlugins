#pragma once

#include <MinimalJuceHeader.h>
#include <jonssonic/effects/compressor.h>
#include <parameters/ParameterManager.h>
#include <visualizers/VisualizerManager.h>
#include "Params.h"
#include "Visualizers.h"


class CompressorAudioProcessor : public juce::AudioProcessor
{
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
    jonssonic::juce_framework::visualizers::VisualizerManager<jonssonic::plugins::compressor::visualizers::ID>& getVisualizerManager() { return visualizerManager; }

private:
    // Bus Properties
    static constexpr int mainInputBus = 0;
    static constexpr int sidechainBus = 1;
    
    // DSP objects and buffers
    jonssonic::effects::Compressor<float> compressor; // Compressor DSP object
    
    // Parameter manager
    jonssonic::juce_framework::parameters::ParameterManager<jonssonic::plugins::compressor::params::ID> parameterManager;

    // Visualizer manager
    jonssonic::juce_framework::visualizers::VisualizerManager<jonssonic::plugins::compressor::visualizers::ID> visualizerManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorAudioProcessor)
};