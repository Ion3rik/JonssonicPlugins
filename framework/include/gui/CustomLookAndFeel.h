// Jonssonic Plugin Framework
// Jonssonic Custom LookAndFeel
// SPDX-License-Identifier: MIT

#pragma once
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../utils/ResourceUtils.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    CustomLookAndFeel(const Jonssonic::ControlPanelConfig* config) {
        // save pointer to config
        this->config = config;

        // Load the default knob strip from the plugin bundle's Resources folder at runtime
        juce::File knobFile = getResourceFile("knobs/JonssonicRotarySlider.png");
        if (knobFile.existsAsFile()) {
            knobStrip = juce::ImageFileFormat::loadFrom(knobFile);
        }
        numFrames = knobStrip.isValid() ? knobStrip.getHeight() / knobStrip.getWidth() : 0;

        // Load the logo from the plugin bundle's Resources folder at runtime
        juce::File logoFile = getResourceFile("logos/Jonssonic_logo.png");
        if (logoFile.existsAsFile()) {
            logo = juce::ImageFileFormat::loadFrom(logoFile);
        }
    }

    /**
     * @brief Draws the main background with gradient, noise overlay, and logo.
     * Caches the generated background for efficient redrawing.
     * @param width Width of the area to draw.
     * @param height Height of the area to draw.
     */
    void generateMainBackground(int width, int height) {
        if (width <= 0 || height <= 0)
            return;
        cachedBackground = juce::Image(juce::Image::ARGB, width, height, true);
        juce::Graphics g(cachedBackground);
        // Main diagonal gradient using base colour from config (or fallback)
        juce::Colour base = config ? config->gradientBaseColour : juce::Colours::darkgrey;
        juce::Colour base2 = base.darker(0.3f);
        juce::ColourGradient grad(
            base, 0, 0,
            base2, width, height,
            false
        );
        g.setGradientFill(grad);
        g.fillRect(juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height));

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
        g.setOpacity(0.20f);
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

    // Call this from paint() to draw the cached background
    void drawCachedMainBackground(juce::Graphics& g) {
        if (cachedBackground.isValid())
            g.drawImageAt(cachedBackground, 0, 0);
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

    void setKnobStrip(const juce::Image& image) {
        knobStrip = image;
        numFrames = knobStrip.isValid() ? knobStrip.getHeight() / knobStrip.getWidth() : 0;
    }

private:
    const Jonssonic::ControlPanelConfig* config = nullptr;
    juce::Image knobStrip;
    int numFrames = 0;
    juce::Image logo;
    juce::Image cachedBackground;
};
