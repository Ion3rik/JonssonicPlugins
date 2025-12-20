#pragma once

#include <gui/CustomLookAndFeel.h>
#include "utils/ResourceUtils.h"

/**
 * @brief Look and Feel class for the Template plugin.
 * Inherits from CustomLookAndFeel and customizes the appearance
 * of the plugin's user interface elements.
 * @note Currently, it overrides the knob strip image with a plugin-specific one if available.
 */

namespace Jonssonic {
class TemplateLookAndFeel : public CustomLookAndFeel {
public:
    TemplateLookAndFeel(const ControlPanelConfig* config)
        : CustomLookAndFeel(config)
    {
        // Override the knob strip with the plugin-specific one from the bundle's Resources folder at runtime
        juce::File knobFile = getResourceFile("knobs/JonssonicRotarySlider_Template.png");
        if (knobFile.existsAsFile()) {
            setKnobStrip(juce::ImageFileFormat::loadFrom(knobFile));
        }
    }
};
} // namespace Jonssonic

