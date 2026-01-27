#pragma once

#include <gui/CustomLookAndFeel.h>
#include <utils/ResourceUtils.h>

class DelayLookAndFeel : public jnsc::juce_interface::CustomLookAndFeel {
  public:
    DelayLookAndFeel(const jnsc::juce_interface::ControlPanelConfig* config) : CustomLookAndFeel(config) {
        // Override the knob strip with the plugin-specific one from the bundle's Resources folder at runtime
        juce::File knobFile = jnsc::juce_interface::getResourceFile("knobs/JonssonicRotarySlider_Delay.png");
        if (knobFile.existsAsFile()) {
            setKnobStrip(juce::ImageFileFormat::loadFrom(knobFile));
        }
    }
};
