#pragma once

#include "utils/ResourceUtils.h"
#include <gui/CustomLookAndFeel.h>

class CompressorLookAndFeel : public jnsc::juce_interface::CustomLookAndFeel {
  public:
    CompressorLookAndFeel(const jnsc::juce_interface::ControlPanelConfig* config)
        : CustomLookAndFeel(config) {
        // Override the knob strip with the plugin-specific one from the bundle's Resources folder
        // at runtime
        juce::File knobFile =
            jnsc::juce_interface::getResourceFile("knobs/JonssonicRotarySlider_Compressor.png");
        if (knobFile.existsAsFile()) {
            setKnobStrip(juce::ImageFileFormat::loadFrom(knobFile));
        }
    }
};
