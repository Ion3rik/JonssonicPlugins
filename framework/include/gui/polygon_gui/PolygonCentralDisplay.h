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

        // Center the full text stack vertically:
        // [JONSSONIC + plugin] ...gap... [param name + param value]
        const float nameRowHeight = paramNameFontHeight + 6.0f;
        const float valueRowHeight = paramValueFontHeight + 6.0f;
        const float interRowGap = 2.0f;
        const float separatorTopPad = 6.0f;
        const float separatorBottomPad = 4.0f;
        const float separatorThickness = 1.2f;

        const float topGroupHeight = content.getHeight() * 0.28f;
        const float middleGap = content.getHeight() * 0.10f;
        const float bottomGroupHeight =
            separatorTopPad + separatorThickness + separatorBottomPad + nameRowHeight + interRowGap + valueRowHeight;
        const float stackHeight = topGroupHeight + middleGap + bottomGroupHeight;

        const float targetTopGroupCenterY = content.getCentreY();
        float stackTopY = targetTopGroupCenterY - (topGroupHeight * 0.5f);
        stackTopY = juce::jlimit(content.getY(), content.getBottom() - stackHeight, stackTopY);

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
            g.setColour(subTextColour.withAlpha(0.66f));
            auto pluginFont = juce::Font(juce::FontOptions("Avenir", pluginFontHeight, juce::Font::plain));
            pluginFont.setTypefaceStyle("Light");
            g.setFont(pluginFont);
            g.drawFittedText(pluginName, pluginArea.toNearestInt(), juce::Justification::centred, 1);
        }

        if (!parameterDetailsVisible)
            return;

        juce::Graphics::ScopedSaveState detailsState(g);
        g.setOpacity(parameterDetailsOpacity);

        const auto separatorY = bottomGroupArea.getY() + separatorTopPad;
        g.setColour(subTextColour.withAlpha(0.45f));
        g.drawLine(bottomGroupArea.getCentreX() - 12.0f,
                   separatorY,
                   bottomGroupArea.getCentreX() + 12.0f,
                   separatorY,
                   1.2f);

        auto paramArea = bottomGroupArea.withTop(separatorY + separatorBottomPad);

        auto paramNameArea = paramArea.removeFromTop(nameRowHeight);
        paramArea.removeFromTop(interRowGap);
        auto paramValueArea = paramArea.removeFromTop(valueRowHeight);
        valueRepaintBounds = paramValueArea.getSmallestIntegerContainer().expanded(1);

        if (g.clipRegionIntersects(paramNameArea.getSmallestIntegerContainer())) {
            g.setColour(textColour.withAlpha(0.95f));
            auto paramNameFont = juce::Font(juce::FontOptions("Avenir", paramNameFontHeight, juce::Font::plain));
            paramNameFont.setTypefaceStyle("Light");
            g.setFont(paramNameFont);
            g.drawFittedText(activeParameterName, paramNameArea.toNearestInt(), juce::Justification::centred, 1);
        }

        if (g.clipRegionIntersects(paramValueArea.getSmallestIntegerContainer())) {
            g.setColour(textColour.withAlpha(0.98f));
            g.setFont(juce::Font(juce::FontOptions("Avenir", paramValueFontHeight, juce::Font::plain)));
            g.drawFittedText(activeParameterValue, paramValueArea.toNearestInt(), juce::Justification::centred, 1);
        }
    }

    void resized() override { valueRepaintBounds = {}; }

    void setParameterDetailsVisible(bool visible) {
        if (parameterDetailsVisible == visible)
            return;
        parameterDetailsVisible = visible;
        repaint();
    }

    void setParameterDetailsOpacity(float opacity) {
        parameterDetailsOpacity = opacity;
        repaint();
    }

    juce::Colour getBrandColour() const noexcept { return textColour.withAlpha(0.95f); }

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
    float parameterDetailsOpacity = 1.0f;
    juce::Rectangle<int> valueRepaintBounds;
    juce::String lastParameterName;

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

    juce::Colour textColour = juce::Colour::fromRGB(242, 205, 168);
    juce::Colour subTextColour = juce::Colour::fromRGB(176, 163, 140);
    juce::Colour outlineColour = juce::Colour::fromRGB(120, 110, 92);

    float panelAlpha = 0.12f;

    float brandFontHeight = 31.0f;
    float pluginFontHeight = 18.0f;
    float paramNameFontHeight = 19.0f;
    float paramValueFontHeight = 19.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolygonCentralDisplay)
};

} // namespace jnsc::juce_interface
