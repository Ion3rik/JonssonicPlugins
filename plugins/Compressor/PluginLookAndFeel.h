#pragma once

#include <gui/CustomLookAndFeel.h>
#include "utils/ResourceUtils.h"

namespace Jonssonic {
class CompressorLookAndFeel : public CustomLookAndFeel {
public:
    CompressorLookAndFeel(const ControlPanelConfig* config)
        : CustomLookAndFeel(config)
    {
        // Override the knob strip with the plugin-specific one from the bundle's Resources folder at runtime
        juce::File knobFile = getResourceFile("knobs/JonssonicRotarySlider_Compressor.png");
        if (knobFile.existsAsFile()) {
            setKnobStrip(juce::ImageFileFormat::loadFrom(knobFile));
        }
    }
};
} // namespace Jonssonic
