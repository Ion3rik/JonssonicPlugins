#pragma once

#include <HexaAmpFontData.h>
#include <juce_graphics/juce_graphics.h>

namespace HexaAmpFonts {

inline const juce::Typeface::Ptr& regular() {
    static const auto typeface = juce::Typeface::createSystemTypefaceFor(
        HexaAmpFontData::JostRegular_ttf, HexaAmpFontData::JostRegular_ttfSize);
    return typeface;
}

inline const juce::Typeface::Ptr& bold() {
    static const auto typeface = juce::Typeface::createSystemTypefaceFor(
        HexaAmpFontData::JostBold_ttf, HexaAmpFontData::JostBold_ttfSize);
    return typeface;
}

inline juce::Font font(float height, bool useBold = false) {
    return juce::Font(juce::FontOptions(useBold ? bold() : regular()).withHeight(height));
}

} // namespace HexaAmpFonts
