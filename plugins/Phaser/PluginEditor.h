#pragma once

#include "PluginLookAndFeel.h"
#include "PluginProcessor.h"
#include <MinimalJuceHeader.h>
#include <gui/ControlPanel.h>
#include <gui/ControlPanelConfig.h>

class PhaserAudioProcessorEditor : public juce::AudioProcessorEditor {
  public:
    PhaserAudioProcessorEditor(PhaserAudioProcessor&);
    ~PhaserAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    PhaserAudioProcessor& audioProcessor;
    jnsc::juce_interface::ControlPanelConfig controlPanelConfig;
    jnsc::juce_interface::ControlPanel controlPanel;

    std::unique_ptr<PhaserLookAndFeel> customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaserAudioProcessorEditor)
};