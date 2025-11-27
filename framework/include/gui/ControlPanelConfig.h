// Jonssonic Plugin Framework
// Control panel configuration for automatic GUI
// SPDX-License-Identifier: MIT

#pragma once
#include <JuceHeader.h>

namespace Jonssonic {

struct ControlPanelConfig
{
    int columns;
    bool showValueBoxes;
    int controlHeight;
    int labelHeight;
    int spacing;

    enum class LabelPosition { Above, Below, Left, Right };
    LabelPosition labelPosition;
    juce::Colour backgroundColor;

    ControlPanelConfig()
        : columns(3),
          showValueBoxes(true),
          controlHeight(100),
          labelHeight(20),
          spacing(10),
          labelPosition(LabelPosition::Above),
          backgroundColor(juce::Colours::transparentBlack)
    {}
};

} // namespace Jonssonic