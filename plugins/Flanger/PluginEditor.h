#pragma once
#include "Version.h"

#include "PluginLookAndFeel.h"
#include "PluginProcessor.h"
#include <MinimalJuceHeader.h>
#include <gui/ControlPanel.h>
#include <gui/ControlPanelConfig.h>

class FlangerAudioProcessorEditor : public juce::AudioProcessorEditor {
  public:
    FlangerAudioProcessorEditor(FlangerAudioProcessor&);
    ~FlangerAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    FlangerAudioProcessor& audioProcessor;
    jnsc::juce_interface::ControlPanelConfig controlPanelConfig;
    jnsc::juce_interface::ControlPanel controlPanel;

    std::unique_ptr<FlangerLookAndFeel> customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlangerAudioProcessorEditor)
};