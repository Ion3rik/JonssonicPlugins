#pragma once
#include "Version.h"
#include <array>

#include "PluginProcessor.h"
#include <MinimalJuceHeader.h>
#include <gui/polygon_gui/PolygonPanel.h>
#include <parameters/ParameterIdUtils.h>

class HexaAmpAudioProcessorEditor : public juce::AudioProcessorEditor {
  public:
    HexaAmpAudioProcessorEditor(HexaAmpAudioProcessor&);
    ~HexaAmpAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    class ThemeButton : public juce::Button {
      public:
        ThemeButton() : juce::Button("Switch light/dark mode") {
            setMouseCursor(juce::MouseCursor::PointingHandCursor);
        }
        void setDark(bool value) { dark = value; repaint(); }
        void paintButton(juce::Graphics& g, bool, bool) override {
            const auto circle = getLocalBounds().toFloat().reduced(5.0f);
            juce::Path mask;
            mask.addEllipse(circle);
            {
                juce::Graphics::ScopedSaveState state(g);
                g.reduceClipRegion(mask);
                g.setColour(dark ? juce::Colours::black : juce::Colours::white);
                g.fillRect(circle);
                g.setColour(dark ? juce::Colours::white : juce::Colours::black);
                g.fillRect(circle.withLeft(circle.getCentreX()));
            }
            g.setColour(dark ? juce::Colours::white : juce::Colours::black);
            g.drawEllipse(circle, 1.0f);
        }
      private:
        bool dark = true;
    };

    class VersionHotspot : public juce::Component {
      public:
        explicit VersionHotspot(HexaAmpAudioProcessorEditor& editor) : editor(editor) {}
        void mouseDoubleClick(const juce::MouseEvent&) override { editor.toggleDevelopmentPanel(); }

      private:
        HexaAmpAudioProcessorEditor& editor;
    };

    class TrimmerLookAndFeel : public juce::LookAndFeel_V4 {
      public:
        bool areScrollbarButtonsVisible() override { return false; }

        void drawScrollbar(juce::Graphics& g, juce::ScrollBar& bar, int x, int y, int width, int height,
                           bool vertical, int thumbStart, int thumbSize, bool, bool) override {
            g.setColour(bar.findColour(juce::ScrollBar::backgroundColourId));
            g.fillRect(x, y, width, height);
            g.setColour(bar.findColour(juce::ScrollBar::thumbColourId));
            if (vertical)
                g.fillRoundedRectangle(static_cast<float>(x + (width - 4) / 2),
                                       static_cast<float>(y + thumbStart), 4.0f, static_cast<float>(thumbSize), 2.0f);
            else
                g.fillRoundedRectangle(static_cast<float>(x + thumbStart),
                                       static_cast<float>(y + (height - 4) / 2), static_cast<float>(thumbSize), 4.0f, 2.0f);
        }

        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                              float position, float startAngle, float endAngle, juce::Slider& slider) override {
            const auto colour = slider.findColour(juce::Slider::rotarySliderFillColourId);
            auto centre = juce::Point<float>(x + width * 0.5f, y + height * 0.5f);
            const float diameter = std::min(40.0f, static_cast<float>(std::min(width, height)));
            auto body = juce::Rectangle<float>(diameter, diameter).withCentre(centre);
            g.setColour(colour);
            g.drawRoundedRectangle(body, 4.0f, 1.0f);
            auto screw = body.reduced(7.0f);
            g.drawEllipse(screw, 1.0f);
            juce::Graphics::ScopedSaveState state(g);
            g.addTransform(juce::AffineTransform::rotation(startAngle + position * (endAngle - startAngle),
                                                         centre.x, centre.y));
            g.setColour(colour);
            g.fillRoundedRectangle(centre.x - 2.0f, centre.y - 9.0f, 4.0f, 18.0f, 1.0f);
            g.setColour(colour);
            g.fillEllipse(centre.x - 1.5f, centre.y - 11.0f, 3.0f, 3.0f);
        }
    };

    class LogoDisplay : public juce::Component {
      public:
        LogoDisplay() { setInterceptsMouseClicks(false, false); }

        void paint(juce::Graphics& g) override {
            if (!mask.isValid() || getWidth() <= 32 || getHeight() <= 32)
                return;
            g.setColour(colour);
            g.drawImage(mask, getLocalBounds().reduced(16).toFloat(),
                        juce::RectanglePlacement::centred, true);
        }

        juce::Image mask;
        juce::Colour colour;

    };

    void applyMonochrome();
    bool darkMode = true;
    ThemeButton themeToggle;
    void loadLogoImage();
    void setPowerState(bool enabled);
    void toggleDevelopmentPanel();

    // We need a reference to the processor object in order to access its parameters
    HexaAmpAudioProcessor& audioProcessor;

    // Polygon parameter panel
    jnsc::juce_interface::PolygonPanel polygonPanel;

    juce::Image logoImage;
    LogoDisplay logoDisplay;
    std::unique_ptr<juce::ParameterAttachment> bypassStateAttachment;
    juce::Component developmentPanel;
    juce::Viewport developmentViewport;
    juce::Label developmentVersion;
    TrimmerLookAndFeel trimmerLookAndFeel;
    std::array<juce::Label, 11> developmentLabels;
    std::array<juce::Slider, 11> developmentSliders;
    VersionHotspot versionHotspot{*this};
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 11> developmentAttachments;
    bool developmentVisible = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HexaAmpAudioProcessorEditor)
};
