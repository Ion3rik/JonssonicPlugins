#pragma once
#include "Version.h"

#include "PluginProcessor.h"
#include <MinimalJuceHeader.h>
#include <gui/ControlPanel.h>
#include <gui/ControlPanelConfig.h>
#include <gui/CustomLookAndFeel.h>

class BiquadDemoAudioProcessorEditor : public juce::AudioProcessorEditor {
  public:
    BiquadDemoAudioProcessorEditor(BiquadDemoAudioProcessor&);
    ~BiquadDemoAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    // We need a reference to the processor object in order to access its parameters
    BiquadDemoAudioProcessor& audioProcessor;

    // Configuration structure for the control panel (NOTE: has to be declared before controlPanel)
    jnsc::juce_interface::ControlPanelConfig controlPanelConfig;

    // Automatic control panel for parameters
    jnsc::juce_interface::ControlPanel controlPanel;

    // Custom look and feel
    std::unique_ptr<jnsc::juce_interface::CustomLookAndFeel> customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BiquadDemoAudioProcessorEditor)
};