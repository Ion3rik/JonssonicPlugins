#pragma once

#include <MinimalJuceHeader.h>
#include <Jonssonic/effects/Compressor.h>
#include <parameters/ParameterManager.h>
#include <visualizers/VisualizerManager.h>
#include "Params.h"
#include "Visualizers.h"


class CompressorAudioProcessor : public juce::AudioProcessor
{
public:
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
    Jonssonic::VisualizerManager<CompressorVisualizers::ID>& getVisualizerManager() { return visualizerManager; }

private:
    // DSP objects and buffers
    Jonssonic::effects::Compressor<float> compressor; // Compressor DSP object
    
    // Parameter manager
    Jonssonic::ParameterManager<CompressorParams::ID> parameterManager;

    // Visualizer manager
    Jonssonic::VisualizerManager<CompressorVisualizers::ID> visualizerManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorAudioProcessor)
};