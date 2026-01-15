#pragma once
#include "Version.h"

#include "PluginLookAndFeel.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>
#include <gui/ControlPanel.h>
#include <gui/ControlPanelConfig.h>

class DistortionAudioProcessorEditor : public juce::AudioProcessorEditor {
  public:
    DistortionAudioProcessorEditor(DistortionAudioProcessor&);
    ~DistortionAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    // We need a reference to the processor object in order to access its parameters
    DistortionAudioProcessor& audioProcessor;

    // Configuration structure for the control panel (NOTE: has to be declared before controlPanel)
    jnsc::juce_interface::ControlPanelConfig controlPanelConfig;

    // Automatic control panel for parameters
    jnsc::juce_interface::ControlPanel controlPanel;

    std::unique_ptr<DistortionLookAndFeel> customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionAudioProcessorEditor)
};