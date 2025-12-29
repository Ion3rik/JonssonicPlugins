#pragma once
#include "Version.h"

#include <MinimalJuceHeader.h>
#include "PluginProcessor.h"
#include <gui/ControlPanel.h>
#include <gui/ControlPanelConfig.h>
#include <gui/GainReductionMeterComponent.h>
#include "PluginLookAndFeel.h"

class CompressorAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    CompressorAudioProcessorEditor(CompressorAudioProcessor&);
    ~CompressorAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Reference to the processor for parameter and visualizer access
    CompressorAudioProcessor& audioProcessor;

    // Control panel config and instance (note the order must be config first here)
    Jonssonic::ControlPanelConfig controlPanelConfig;
    Jonssonic::ControlPanel controlPanel;

    // Level meter for gain reduction
    std::unique_ptr<Jonssonic::GainReductionMeterComponent> gainReductionMeter;

    // Custom look and feel
    std::unique_ptr<Jonssonic::CompressorLookAndFeel> customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorAudioProcessorEditor)
};