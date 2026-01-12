#pragma once

#include <gui/CustomLookAndFeel.h>
#include "utils/ResourceUtils.h"

namespace jonssonic::plugins::compressor {
class CompressorLookAndFeel : public juce_framework::gui::CustomLookAndFeel {
public:
    CompressorLookAndFeel(const juce_framework::gui::ControlPanelConfig* config)
        : CustomLookAndFeel(config)
    {
        // Override the knob strip with the plugin-specific one from the bundle's Resources folder at runtime
        juce::File knobFile = juce_framework::utils::getResourceFile("knobs/JonssonicRotarySlider_Compressor.png");
        if (knobFile.existsAsFile()) {
            setKnobStrip(juce::ImageFileFormat::loadFrom(knobFile));
        }
    }
};
} // namespace Jonssonic
