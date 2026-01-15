#pragma once
#include "Version.h"

#include "PluginLookAndFeel.h"
#include "PluginProcessor.h"
#include <MinimalJuceHeader.h>
#include <gui/ControlPanel.h>
#include <gui/ControlPanelConfig.h>
#include <gui/GainReductionMeterComponent.h>

class CompressorAudioProcessorEditor : public juce::AudioProcessorEditor {
  public:
    CompressorAudioProcessorEditor(CompressorAudioProcessor&);
    ~CompressorAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    // Reference to the processor for parameter and visualizer access
    CompressorAudioProcessor& audioProcessor;

    // Control panel config and instance (note the order must be config first here)
    jnsc::juce_interface::ControlPanelConfig controlPanelConfig;
    jnsc::juce_interface::ControlPanel controlPanel;

    // Level meter for gain reduction
    std::unique_ptr<jnsc::juce_interface::GainReductionMeterComponent> gainReductionMeter;
    // Custom look and feel
    std::unique_ptr<CompressorLookAndFeel> customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorAudioProcessorEditor)
};