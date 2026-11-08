#pragma once

#include <HexaGuitarFontData.h>
#include <juce_graphics/juce_graphics.h>

namespace HexaGuitarFonts {

inline const juce::Typeface::Ptr& regular() {
    static const auto typeface = juce::Typeface::createSystemTypefaceFor(
        HexaGuitarFontData::JostRegular_ttf, HexaGuitarFontData::JostRegular_ttfSize);
    return typeface;
}

inline const juce::Typeface::Ptr& bold() {
    static const auto typeface = juce::Typeface::createSystemTypefaceFor(
        HexaGuitarFontData::JostBold_ttf, HexaGuitarFontData::JostBold_ttfSize);
    return typeface;
}

inline juce::Font font(float height, bool useBold = false) {
    return juce::Font(juce::FontOptions(useBold ? bold() : regular()).withHeight(height));
}

} // namespace HexaGuitarFonts
