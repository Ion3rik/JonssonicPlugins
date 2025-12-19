#pragma once

#include <gui/CustomLookAndFeel.h>

namespace Jonssonic {
class CompressorLookAndFeel : public CustomLookAndFeel {
public:
    CompressorLookAndFeel(const ControlPanelConfig* config)
        : CustomLookAndFeel(config)
    {
        // Override the knob strip with the plugin-specific one from the bundle's Resources folder at runtime
        juce::File knobFile = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
            .getChildFile("Contents/Resources/knobs/JonssonicRotarySlider_Compressor.png");
        if (knobFile.existsAsFile()) {
            setKnobStrip(juce::ImageFileFormat::loadFrom(knobFile));
        }
    }
};
} // namespace Jonssonic
