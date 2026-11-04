// Jonssonic Plugin Framework
// Polygon central display
// SPDX-License-Identifier: MIT

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace jnsc::juce_interface {

/**
 * @brief Center information display used inside PolygonPanel.
 *
 * Renders:
 * - Brand text (e.g. "J O N S S O N I C")
 * - Plugin name
 * - Last active parameter name and value
 */
class PolygonCentralDisplay : public juce::Component {
  public:
    PolygonCentralDisplay() { setOpaque(false); }

    ~PolygonCentralDisplay() override = default;

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        if (bounds.isEmpty())
            return;

        auto content = bounds.reduced(10.0f);

        // Anchor the heading at the panel center; details extend below it.
        const float nameRowHeight = paramNameFontHeight + 6.0f;
        const float valueRowHeight = paramValueFontHeight + 6.0f;
        const float interRowGap = 2.0f;
        const float separatorTopPad = 6.0f;
        const float separatorBottomPad = 4.0f;
        const float separatorThickness = 1.2f;

        const float topGroupHeight = content.getWidth() * 0.28f;
        const float middleGap = content.getWidth() * 0.10f;
        const float bottomGroupHeight =
            separatorTopPad + separatorThickness + separatorBottomPad + nameRowHeight + interRowGap + valueRowHeight;

        const float targetTopGroupCenterY = content.getCentreY();
        const float stackTopY = targetTopGroupCenterY - (topGroupHeight * 0.5f);

        auto topGroupArea = juce::Rectangle<float>(content.getX(), stackTopY, content.getWidth(), topGroupHeight);
        auto bottomGroupArea = juce::Rectangle<float>(content.getX(),
                                                      stackTopY + topGroupHeight + middleGap,
                                                      content.getWidth(),
                                                      bottomGroupHeight);

        auto brandArea = topGroupArea.removeFromTop(topGroupArea.getHeight() * 0.72f);
        auto pluginArea = topGroupArea;

        if (g.clipRegionIntersects(brandArea.getSmallestIntegerContainer())) {
            g.setColour(getBrandColour());
            g.setFont(juce::Font(juce::FontOptions("Avenir", brandFontHeight, juce::Font::plain)));
            g.drawFittedText(brandText, brandArea.toNearestInt(), juce::Justification::centred, 1);
        }

        if (g.clipRegionIntersects(pluginArea.getSmallestIntegerContainer())) {
            g.setColour(subTextColour);
            auto pluginFont = juce::Font(juce::FontOptions("Avenir", pluginFontHeight, juce::Font::plain));
            pluginFont.setTypefaceStyle("Light");
            g.setFont(pluginFont);
            g.drawFittedText(pluginName, pluginArea.toNearestInt(), juce::Justification::centred, 1);
        }

        if (!parameterDetailsVisible)
            return;

        const auto separatorY = bottomGroupArea.getY() + separatorTopPad;
        g.setColour(subTextColour);
        g.drawLine(bottomGroupArea.getCentreX() - 12.0f,
                   separatorY,
                   bottomGroupArea.getCentreX() + 12.0f,
                   separatorY,
                   1.2f);

        auto paramArea = bottomGroupArea.withTop(separatorY + separatorBottomPad);

        auto paramNameArea = paramArea.removeFromTop(nameRowHeight);
        paramArea.removeFromTop(interRowGap);
        auto paramValueArea = paramArea.removeFromTop(valueRowHeight);
        const auto valueContentArea = helpText.isEmpty()
                                          ? paramValueArea
                                          : paramValueArea.withHeight(juce::jmax(64.0f, content.getWidth() * 0.34f));
        valueRepaintBounds = valueContentArea.getSmallestIntegerContainer().expanded(1);

        if (g.clipRegionIntersects(paramNameArea.getSmallestIntegerContainer())) {
            g.setColour(textColour);
            auto paramNameFont = juce::Font(juce::FontOptions("Avenir", paramNameFontHeight, juce::Font::plain));
            paramNameFont.setTypefaceStyle("Light");
            g.setFont(paramNameFont);
            g.drawFittedText(activeParameterName, paramNameArea.toNearestInt(), juce::Justification::centred, 1);
        }

        if (g.clipRegionIntersects(valueContentArea.getSmallestIntegerContainer())) {
            g.setColour(textColour);
            if (helpText.isNotEmpty()) {
                auto helpFont = juce::Font(juce::FontOptions("Avenir", 14.0f, juce::Font::plain));
                helpFont.setTypefaceStyle("Light");
                g.setFont(helpFont);
                g.drawFittedText(helpText, valueContentArea.toNearestInt(), juce::Justification::centredTop, 5, 0.85f);
            } else {
                g.setFont(juce::Font(juce::FontOptions("Avenir", paramValueFontHeight, juce::Font::plain)));
                g.drawFittedText(activeParameterValue, paramValueArea.toNearestInt(), juce::Justification::centred, 1);
            }
        }
    }

    void resized() override { valueRepaintBounds = {}; }

    void setHelpText(const juce::String& text) {
        if (helpText == text)
            return;
        helpText = text;
        valueRepaintBounds = {};
        repaint();
    }

    void setParameterDetailsVisible(bool visible) {
        if (parameterDetailsVisible == visible)
            return;
        parameterDetailsVisible = visible;
        repaint();
    }

    juce::Colour getBrandColour() const noexcept { return textColour; }

    void setBrandText(const juce::String& newBrandText) {
        const auto spaced = toSpacedLetters(newBrandText, true);
        if (brandText == spaced)
            return;
        brandText = spaced;
        repaint();
    }

    void setPluginName(const juce::String& newPluginName) {
        const auto spaced = toSpacedLetters(newPluginName, true);
        if (pluginName == spaced)
            return;
        pluginName = spaced;
        repaint();
    }

    void setActiveParameter(const juce::String& paramName, const juce::String& valueText) {
        const auto resolvedName = paramName == lastParameterName
                                      ? activeParameterName
                                      : (paramName.trim().isNotEmpty() ? toSpacedLetters(paramName, true)
                                                                      : juce::String("P A R A M"));
        const auto resolvedValue = valueText.trim().isNotEmpty() ? valueText : juce::String("--");

        if (activeParameterName == resolvedName && activeParameterValue == resolvedValue)
            return;

        const bool nameChanged = activeParameterName != resolvedName;
        lastParameterName = paramName;
        activeParameterName = resolvedName;
        activeParameterValue = resolvedValue;
        if (nameChanged || valueRepaintBounds.isEmpty())
            repaint();
        else
            repaint(valueRepaintBounds);
    }

    void clearActiveParameter() {
        lastParameterName.clear();
        activeParameterName = defaultParameterName;
        activeParameterValue = defaultParameterValue;
        repaint();
    }

    void setPalette(juce::Colour newTextColour, juce::Colour newSubTextColour, juce::Colour newOutlineColour) {
        textColour = newTextColour;
        subTextColour = newSubTextColour;
        outlineColour = newOutlineColour;
        repaint();
    }

    void setPanelAlpha(float newAlpha) {
        panelAlpha = juce::jlimit(0.0f, 1.0f, newAlpha);
        repaint();
    }

  private:
    bool parameterDetailsVisible = true;
    juce::Rectangle<int> valueRepaintBounds;
    juce::String lastParameterName;
    juce::String helpText;

    static juce::String toSpacedLetters(const juce::String& source, bool uppercase) {
        juce::String text = source.trim();
        if (uppercase)
            text = text.toUpperCase();

        juce::String compact;
        for (int i = 0; i < text.length(); ++i) {
            const juce::juce_wchar c = text[i];
            if (!juce::CharacterFunctions::isWhitespace(c))
                compact << juce::String::charToString(c);
        }

        juce::String spaced;
        for (int i = 0; i < compact.length(); ++i) {
            if (i > 0)
                spaced << " ";
            spaced << juce::String::charToString(compact[i]);
        }

        return spaced;
    }

    juce::String brandText = "J O N S S O N I C";
    juce::String pluginName = "PLUGIN";

    const juce::String defaultParameterName = "PARAM";
    const juce::String defaultParameterValue = "--";
    juce::String activeParameterName = defaultParameterName;
    juce::String activeParameterValue = defaultParameterValue;

    juce::Colour textColour = juce::Colours::white;
    juce::Colour subTextColour = juce::Colours::white;
    juce::Colour outlineColour = juce::Colours::white;

    float panelAlpha = 0.12f;

    float brandFontHeight = 31.0f;
    float pluginFontHeight = 18.0f;
    float paramNameFontHeight = 19.0f;
    float paramValueFontHeight = 19.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolygonCentralDisplay)
};

} // namespace jnsc::juce_interface
