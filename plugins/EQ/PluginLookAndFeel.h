#pragma once

#include <gui/CustomLookAndFeel.h>

namespace Jonssonic {
class EQLookAndFeel : public CustomLookAndFeel {
public:
    EQLookAndFeel(const ControlPanelConfig* config)
        : CustomLookAndFeel(config)
    {
        // Override the knob strip with the plugin-specific one from the bundle's Resources folder at runtime
        juce::File knobFile = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
            .getChildFile("Contents/Resources/knobs/JonssonicRotarySlider_EQ.png");
        if (knobFile.existsAsFile()) {
            setKnobStrip(juce::ImageFileFormat::loadFrom(knobFile));
        }
    }
};
} // namespace Jonssonic

