#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <gui/ControlPanel.h>
#include <gui/ControlPanelConfig.h>

class FlangerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    FlangerAudioProcessorEditor(FlangerAudioProcessor&);
    ~FlangerAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    FlangerAudioProcessor& audioProcessor;
    Jonssonic::ControlPanelConfig controlPanelConfig;
    Jonssonic::ControlPanel controlPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlangerAudioProcessorEditor)
};