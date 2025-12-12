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
        
        // Load the logo from embedded binary data
        logo = juce::ImageCache::getFromMemory(BinaryData::Jonssonic_logo_v2_png, BinaryData::Jonssonic_logo_v2_pngSize);
    }
    
    // Unified main background for editors
    void drawMainBackground(juce::Graphics& g, int width, int height, const Jonssonic::ControlPanelConfig* config = nullptr) {
        // Main diagonal gradient
        juce::ColourGradient grad(
            juce::Colour(0xff555555), 0, 0,
            juce::Colour(0xff444444), width, height,
            false
        );
        g.setGradientFill(grad);
        float cornerRadius = 16.0f;
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
        
        // Draw logo with configurable placement
        if (logo.isValid() && config) {
            const float logoOpacity = 1.0f;
            int logoHeight = static_cast<int>(config->logoHeight);
            
            int logoWidth = logoHeight; // Always square for now

                int logoX = config->logoMarginX;
                int logoY = static_cast<int>(config->logoMarginY);

            // Horizontal placement
            switch (config->logoHorizontalPlacement) {
                case Jonssonic::ControlPanelConfig::HorizontalPlacement::Left:
                        logoX = config->logoMarginX;
                    break;
                case Jonssonic::ControlPanelConfig::HorizontalPlacement::Center:
                        logoX = (width - logoWidth) / 2 + config->logoMarginX;
                    break;
                case Jonssonic::ControlPanelConfig::HorizontalPlacement::Right:
                        logoX = width - logoWidth - config->logoMarginX;
                    break;
            }

            // Vertical placement
            switch (config->logoVerticalPlacement) {
                case Jonssonic::ControlPanelConfig::VerticalPlacement::Top:
                        logoY = config->logoMarginY;
                    break;
                case Jonssonic::ControlPanelConfig::VerticalPlacement::Center:
                        logoY = (height - logoHeight) / 2 + config->logoMarginY;
                    break;
                case Jonssonic::ControlPanelConfig::VerticalPlacement::Bottom:
                        logoY = height - logoHeight - config->logoMarginY;
                    break;
            }

            g.setOpacity(logoOpacity);
            // Draw logo with exact dimensions, ignoring aspect ratio
            g.drawImage(logo, 
                       (float)logoX, (float)logoY, (float)logoWidth, (float)logoHeight,
                       0, 0, logo.getWidth(), logo.getHeight(), false);
            g.setOpacity(1.0f);
        }
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
    {
        if (knobStrip.isValid() && numFrames > 0) {
            const int frameSize = knobStrip.getWidth();
            const int frame = juce::jlimit(0, numFrames - 1, static_cast<int>(std::round(sliderPosProportional * (numFrames - 1))));
            const float scale = 1.0f;
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

        // Text with font from label's property (set by ControlPanel)
        g.setColour (label.findColour (juce::Label::textColourId));
        g.setFont (label.getFont());
        g.drawFittedText (text, label.getLocalBounds(), label.getJustificationType(), 1);
    }

private:
    juce::Image knobStrip;
    int numFrames = 0;
    juce::Image logo;
};
