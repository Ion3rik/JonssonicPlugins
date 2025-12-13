#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <gui/ControlPanel.h>
#include <gui/ControlPanelConfig.h>
#include <gui/CustomLookAndFeel.h>

class EQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    EQAudioProcessorEditor(EQAudioProcessor&);
    ~EQAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:

    // We need a reference to the processor object in order to access its parameters
    EQAudioProcessor& audioProcessor;

    // Configuration structure for the control panel (NOTE: has to be declared before controlPanel)
    Jonssonic::ControlPanelConfig controlPanelConfig;

    // Automatic control panel for parameters
    Jonssonic::ControlPanel controlPanel;

    std::unique_ptr<CustomLookAndFeel> customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQAudioProcessorEditor)
};