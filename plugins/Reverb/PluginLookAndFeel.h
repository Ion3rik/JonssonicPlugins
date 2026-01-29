#pragma once

#include "utils/ResourceUtils.h"
#include <gui/CustomLookAndFeel.h>

class ReverbLookAndFeel : public jnsc::juce_interface::CustomLookAndFeel {
  public:
    ReverbLookAndFeel(const jnsc::juce_interface::ControlPanelConfig* config) : CustomLookAndFeel(config) {
        // Override the knob strip with the plugin-specific one from the bundle's Resources folder at runtime
        juce::File knobFile = jnsc::juce_interface::getResourceFile("knobs/JonssonicRotarySlider_Reverb.png");
        if (knobFile.existsAsFile()) {
            setKnobStrip(juce::ImageFileFormat::loadFrom(knobFile));
        }
    }
};
