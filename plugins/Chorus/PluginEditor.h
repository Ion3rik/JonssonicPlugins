#pragma once
#include "Version.h"

#include "PluginLookAndFeel.h"
#include "PluginProcessor.h"
#include <MinimalJuceHeader.h>
#include <gui/ControlPanel.h>
#include <gui/ControlPanelConfig.h>

class ChorusAudioProcessorEditor : public juce::AudioProcessorEditor {
  public:
    ChorusAudioProcessorEditor(ChorusAudioProcessor&);
    ~ChorusAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    // We need a reference to the processor object in order to access its parameters
    ChorusAudioProcessor& audioProcessor;

    // Configuration structure for the control panel (NOTE: has to be declared before controlPanel)
    jnsc::juce_interface::ControlPanelConfig controlPanelConfig;

    // Automatic control panel for parameters
    jnsc::juce_interface::ControlPanel controlPanel;

    std::unique_ptr<TemplateLookAndFeel> customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusAudioProcessorEditor)
};