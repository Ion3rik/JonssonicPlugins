// Jonssonic Plugin Framework
// Jonssonic Custom LookAndFeel
// SPDX-License-Identifier: MIT

#pragma once
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    CustomLookAndFeel() {
        // Load the knob strip from embedded binary data
        knobStrip = juce::ImageCache::getFromMemory(BinaryData::JonssonicRotarySlider_png, BinaryData::JonssonicRotarySlider_pngSize);
        numFrames = knobStrip.isValid() ? knobStrip.getHeight() / knobStrip.getWidth() : 0;
    }
    // Unified main background for editors
    static void drawMainBackground(juce::Graphics& g, int width, int height) {
        // Main diagonal gradient
        juce::ColourGradient grad(
            juce::Colour(0xff555555), 0, 0,
            juce::Colour(0xff333333), width, height,
            false
        );
        g.setGradientFill(grad);
        float cornerRadius = 16.0f;
        g.fillRoundedRectangle(0, 0, (float)width, (float)height, cornerRadius);

        // Vignette (radial gradient, transparent center, dark edges)
        juce::ColourGradient vignette(
            juce::Colours::transparentBlack, width / 2.0f, height / 2.0f,
            juce::Colour(0x22000000), width / 2.0f, 0.0f, true // much lower alpha
        );
        vignette.addColour(0.9, juce::Colour(0x22000000)); // fade out closer to edge
        vignette.addColour(1.0, juce::Colour(0x44000000)); // outer edge, still subtle
        g.setGradientFill(vignette);
        g.fillRoundedRectangle(0, 0, (float)width, (float)height, cornerRadius);

        // Noise overlay (simple random dots)
        juce::Image noise(juce::Image::ARGB, width, height, true);
        juce::Graphics ng(noise);
        juce::Random rng;
        for (int i = 0; i < width * height / 8; ++i) {
            float alpha = 0.04f + 0.04f * rng.nextFloat();
            juce::Colour c = juce::Colour(0xffffffff).withAlpha(alpha);
            int nx = rng.nextInt(width);
            int ny = rng.nextInt(height);
            ng.setColour(c);
            ng.fillRect(nx, ny, 1, 1);
        }
        g.setOpacity(0.10f);
        g.drawImageAt(noise, 0, 0);
        g.setOpacity(1.0f);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
    {
        if (knobStrip.isValid() && numFrames > 0) {
            const int frameSize = knobStrip.getWidth();
            const int frame = juce::jlimit(0, numFrames - 1, static_cast<int>(std::round(sliderPosProportional * (numFrames - 1))));
            const float scale = 0.8f;
            const int baseSize = std::min(width, height);
            const int size = static_cast<int>(baseSize * scale);
            const int cx = x + (width - size) / 2;
            const int cy = y + (height - size) / 2;
            g.drawImage(knobStrip,
                        cx, cy, size, size, // dest: centered square, scaled
                        0, frame * frameSize, frameSize, frameSize // source
            );
        } else {
            // fallback: default rotary
            juce::LookAndFeel_V4::drawRotarySlider(g, x, y, width, height, sliderPosProportional, rotaryStartAngle, rotaryEndAngle, slider);
        }
    }

    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        auto& lf = getDefaultLookAndFeel();
        auto text = label.getText();
        auto area = label.getLocalBounds().toFloat();

        // Background
        g.setColour (label.findColour (juce::Label::backgroundColourId));
        g.fillRoundedRectangle (area, 6.0f);  // <-- rounded corners

        // Border
        g.setColour (label.findColour (juce::Label::outlineColourId));
        g.drawRoundedRectangle (area, 6.0f, 1.0f);

        // Text
        g.setColour (label.findColour (juce::Label::textColourId));
        g.setFont (label.getFont());
        g.drawFittedText (text, label.getLocalBounds(), label.getJustificationType(), 1);
    }

private:
    juce::Image knobStrip;
    int numFrames = 0;
};
