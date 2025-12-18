#pragma once

#include <framework/include/gui/CustomLookAndFeel.h>
#include "TemplateBinaryData.h"

class TemplateLookAndFeel : public CustomLookAndFeel {
public:
    TemplateLookAndFeel(const Jonssonic::ControlPanelConfig* config)
        : CustomLookAndFeel(config)
    {
        // Override the knob strip with the plugin-specific one
        knobStrip = juce::ImageCache::getFromMemory(TemplateBinaryData::JonssonicRotarySlider_png, TemplateBinaryData::JonssonicRotarySlider_pngSize);
        numFrames = knobStrip.isValid() ? knobStrip.getHeight() / knobStrip.getWidth() : 0;
    }
};
