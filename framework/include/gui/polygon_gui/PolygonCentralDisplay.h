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

        const float topGroupHeight = brandFontHeight + pluginFontHeight + 22.0f;
        const float middleGap = content.getWidth() * 0.10f;
        const float bottomGroupHeight =
            nameRowHeight + interRowGap + valueRowHeight;

        const float targetTopGroupCenterY = content.getCentreY();
        const float stackTopY = targetTopGroupCenterY - (topGroupHeight * 0.5f);

        auto topGroupArea = juce::Rectangle<float>(content.getX(), stackTopY, content.getWidth(), topGroupHeight);
        auto bottomGroupArea = juce::Rectangle<float>(content.getX(),
                                                      stackTopY + topGroupHeight + middleGap,
                                                      content.getWidth(),
                                                      bottomGroupHeight);

        auto brandArea = topGroupArea.removeFromTop(brandFontHeight + 6.0f);
        topGroupArea.removeFromTop(6.0f);
        auto pluginArea = topGroupArea;

        if (g.clipRegionIntersects(brandArea.getSmallestIntegerContainer())) {
            g.setColour(getBrandColour());
            g.setFont(juce::Font(juce::FontOptions("Avenir", brandFontHeight, juce::Font::plain)));
            g.drawFittedText(brandText, brandArea.toNearestInt(), juce::Justification::centred, 1);
        }

        if (g.clipRegionIntersects(pluginArea.getSmallestIntegerContainer())) {
            g.setColour(subTextColour);
            if (useHexaWordmark) {
                drawHexaWordmark(g, pluginArea);
            } else {
                g.setFont(juce::Font(juce::FontOptions("Avenir", pluginFontHeight, juce::Font::plain)));
                g.drawFittedText(pluginName, pluginArea.toNearestInt(), juce::Justification::centred, 1);
            }
        }

        if (!parameterDetailsVisible)
            return;

        auto paramArea = bottomGroupArea;

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
            g.setFont(paramNameFont);
            g.drawFittedText(helpTitle.isEmpty() ? activeParameterName : helpTitle, paramNameArea.toNearestInt(), juce::Justification::centred, 1);
        }

        if (g.clipRegionIntersects(valueContentArea.getSmallestIntegerContainer())) {
            g.setColour(textColour);
            if (helpText.isNotEmpty()) {
                auto helpFont = juce::Font(juce::FontOptions("Avenir", 14.0f, juce::Font::plain));
                g.setFont(helpFont);
                g.drawFittedText(helpText, valueContentArea.toNearestInt(), juce::Justification::centredTop, 5, 0.85f);
            } else {
                g.setFont(juce::Font(juce::FontOptions("Avenir", paramValueFontHeight, juce::Font::plain)));
                g.drawFittedText(activeParameterValue, paramValueArea.toNearestInt(), juce::Justification::centred, 1);
            }
        }
    }

    void resized() override { valueRepaintBounds = {}; }

    void setHelpText(const juce::String& text, const juce::String& title = {}) {
        const auto resolvedTitle = title.isEmpty() ? juce::String{} : toSpacedLetters(title, true);
        if (helpText == text && helpTitle == resolvedTitle)
            return;
        helpText = text;
        helpTitle = resolvedTitle;
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
        const auto rawName = newPluginName.trim();
        useHexaWordmark = rawName.startsWithIgnoreCase("HEXA") && rawName.length() > 4;
        productName = useHexaWordmark ? rawName.substring(4).replaceCharacter('-', ' ').replaceCharacter('_', ' ').trim().toUpperCase()
                                     : juce::String{};
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
        textColour = newTextColour.withAlpha(1.0f);
        subTextColour = newSubTextColour.withAlpha(1.0f);
        outlineColour = newOutlineColour.withAlpha(1.0f);
        repaint();
    }

  private:
    void drawHexaWordmark(juce::Graphics& g, juce::Rectangle<float> area) {
        const juce::Font familyFont(juce::FontOptions("Avenir", pluginFontHeight, juce::Font::plain));
        const juce::Font productFont(juce::FontOptions("Avenir", pluginFontHeight, juce::Font::bold));
        juce::GlyphArrangement lettering;
        lettering.addLineOfText(familyFont, "HEXA ", 0.0f, 0.0f);
        const float familyWidth = juce::GlyphArrangement::getStringWidth(familyFont, "HEXA ");
        lettering.addLineOfText(productFont, productName, familyWidth, 0.0f);
        const auto inkBounds = lettering.getBoundingBox(0, -1, true);
        const float scale = juce::jmin(1.0f, (area.getWidth() - 8.0f) / juce::jmax(1.0f, inkBounds.getWidth()));
        const auto transform = juce::AffineTransform::translation(-inkBounds.getCentreX(), -inkBounds.getCentreY())
                                   .scaled(scale)
                                   .translated(area.getCentreX(), area.getCentreY());
        lettering.draw(g, transform);
    }

    juce::String productName;
    bool useHexaWordmark = false;
    bool parameterDetailsVisible = true;
    juce::Rectangle<int> valueRepaintBounds;
    juce::String lastParameterName;
    juce::String helpText;
    juce::String helpTitle;

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

    float brandFontHeight = 22.0f;
    float pluginFontHeight = 28.0f;
    float paramNameFontHeight = 19.0f;
    float paramValueFontHeight = 19.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolygonCentralDisplay)
};

} // namespace jnsc::juce_interface
