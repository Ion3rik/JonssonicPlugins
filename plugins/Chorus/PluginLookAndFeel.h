#pragma once

#include "utils/ResourceUtils.h"
#include <gui/CustomLookAndFeel.h>

/**
 * @brief Look and Feel class for the Chorus plugin.
 * Inherits from CustomLookAndFeel and customizes the appearance
 * of the plugin's user interface elements.
 * @note Currently, it overrides the knob strip image with a plugin-specific one if available.
 */

class ChorusLookAndFeel : public jnsc::juce_interface::CustomLookAndFeel {
  public:
    ChorusLookAndFeel(const jnsc::juce_interface::ControlPanelConfig* config) : CustomLookAndFeel(config) {
        // Override the knob strip with the plugin-specific one from the bundle's Resources folder at runtime
        juce::File knobFile = jnsc::juce_interface::getResourceFile("knobs/JonssonicRotarySlider_Chorus.png");
        if (knobFile.existsAsFile()) {
            setKnobStrip(juce::ImageFileFormat::loadFrom(knobFile));
        }
    }
};
