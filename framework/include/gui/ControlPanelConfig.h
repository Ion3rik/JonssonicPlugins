// Jonssonic Plugin Framework
// Control panel configuration for automatic GUI
// SPDX-License-Identifier: MIT

#pragma once

#include <juce_graphics/juce_graphics.h> 
#include <juce_core/juce_core.h>         


namespace Jonssonic {

/**
 * @brief Configuration structure for Jonssonic ControlPanel automatic GUI.
 *
 * This struct controls the layout, appearance, and labeling of the plugin control panel.
 * All fields are public and can be set directly or via aggregate initialization.
 *
 * Example usage:
 *   ControlPanelConfig config;
 *   config.columns = 3;
 *   config.title = "AMAZING PLUGIN";
 *   config.titleHeight = 60;
 *   config.titleHorizontalPlacement = ControlPanelConfig::HorizontalPlacement::Center;
 *   config.logoVerticalPlacement = ControlPanelConfig::VerticalPlacement::Top;
 *   config.logoHorizontalPlacement = ControlPanelConfig::HorizontalPlacement::Right;
 *   config.logoSize = 80;
 */
struct ControlPanelConfig
{
    /** Number of columns in the control grid. */
    int columns;
    /** Show value boxes below sliders. */
    bool showValueBoxes;
    /** Height of each control (slider, combo, button) in pixels. */
    int controlHeight;
    /** Height of parameter labels in pixels. */
    int labelHeight;
    /** Spacing between controls in pixels. */
    int spacing;

    /** Main title text for the panel. */
    juce::String title;
    /** Height of the title area in pixels. */
    int titleHeight;
    /** Subtitle text for the panel. */
    juce::String subtitle;
    /** Height of the subtitle area in pixels. */
    int subtitleHeight;

    /** Vertical placement for title, subtitle, and logo. */
    enum class VerticalPlacement { Top, Center, Bottom };
    /** Horizontal placement for title, subtitle, and logo. */
    enum class HorizontalPlacement { Left, Center, Right };


    /** Title vertical placement (Top, Center, Bottom). */
    VerticalPlacement titleVerticalPlacement;
    /** Title horizontal placement (Left, Center, Right). */
    HorizontalPlacement titleHorizontalPlacement;
    /** Subtitle vertical placement. */
    VerticalPlacement subtitleVerticalPlacement;
    /** Subtitle horizontal placement. */
    HorizontalPlacement subtitleHorizontalPlacement;
    /** Title horizontal margin in pixels (space before and after). */
    int titleMarginX;
    /** Title vertical margin in pixels (space above and below). */
    int titleMarginY;
    /** Subtitle horizontal margin in pixels (space before and after). */
    int subtitleMarginX;
    /** Subtitle vertical margin in pixels (space above and below). */
    int subtitleMarginY;

    /** Logo vertical placement. */
    VerticalPlacement logoVerticalPlacement;
    /** Logo horizontal placement. */
    HorizontalPlacement logoHorizontalPlacement;
    /** Logo horizontal margin in pixels (space before and after). */
    int logoMarginX;
    /** Logo vertical margin in pixels (space above and below). */
    int logoMarginY;
    /** Logo height in pixels (0 = auto, matches titleHeight if set). */
    int logoHeight;

    /** Panel background color. */
    juce::Colour backgroundColor;
    /** Base colour for background gradient. */
    juce::Colour gradientBaseColour;
    /** Font name for labels and title. */
    juce::String fontName;
    /** Font style for labels/title (juce::Font::plain, bold, italic). */
    int fontStyle;

    /**
     * @brief Default constructor initializes all fields to sensible defaults.
     */
        ControlPanelConfig()
                : columns(3),
                    showValueBoxes(true),
                    controlHeight(80),
                    labelHeight(15),
                    spacing(0),
                    title("JONSSONIC"),
                    titleHeight(40),
                    subtitle("PLUGIN"),
                    subtitleHeight(20),
                    titleVerticalPlacement(VerticalPlacement::Top),
                    titleHorizontalPlacement(HorizontalPlacement::Left),
                    subtitleVerticalPlacement(VerticalPlacement::Bottom),
                    subtitleHorizontalPlacement(HorizontalPlacement::Right),
                    titleMarginX(60),
                    titleMarginY(15),
                    subtitleMarginX(30),
                    subtitleMarginY(15),
                    logoVerticalPlacement(VerticalPlacement::Top),
                    logoHorizontalPlacement(HorizontalPlacement::Left),
                    logoMarginX(15),
                    logoMarginY(15),
                    logoHeight(40),
                    backgroundColor(juce::Colours::transparentBlack),
                    gradientBaseColour(juce::Colours::darkgrey),
                    fontName("Avenir"),
                    fontStyle(juce::Font::plain)
        {}
};

} // namespace Jonssonic