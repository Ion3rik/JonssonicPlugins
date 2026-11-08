#pragma once

#include "Params.h"
#include "DSP/HexaGuitar.h"
#include <MinimalJuceHeader.h>
#include <parameters/ParameterManager.h>

class HexaGuitarAudioProcessor : public juce::AudioProcessor {
  public:
    HexaGuitarAudioProcessor();
    ~HexaGuitarAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    juce::AudioProcessorParameter* getBypassParameter() const override { return bypassParameter; }

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
    jnsc::hexa_guitar::HexaGuitar hexaGuitar; // Amp effect processor
    bool effectPrepared = false;

    // Parameter manager
    jnsc::juce_interface::ParameterManager<HexaGuitarParams::ID> parameterManager;
    juce::RangedAudioParameter* bypassParameter = nullptr;
    bool bypassed = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HexaGuitarAudioProcessor)
};
