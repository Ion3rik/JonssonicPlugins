#pragma once
#include "Version.h"
#include <array>

#include "PluginProcessor.h"
#include <MinimalJuceHeader.h>
#include <gui/polygon_gui/PolygonPanel.h>
#include <parameters/ParameterIdUtils.h>

class HexaAmpAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer {
  public:
    HexaAmpAudioProcessorEditor(HexaAmpAudioProcessor&);
    ~HexaAmpAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    class VersionHotspot : public juce::Component {
      public:
        explicit VersionHotspot(HexaAmpAudioProcessorEditor& editor) : editor(editor) {}
        void mouseDoubleClick(const juce::MouseEvent&) override { editor.toggleDevelopmentPanel(); }

      private:
        HexaAmpAudioProcessorEditor& editor;
    };

    class TrimmerLookAndFeel : public juce::LookAndFeel_V4 {
      public:
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                              float position, float startAngle, float endAngle, juce::Slider& slider) override {
            const auto colour = slider.findColour(juce::Slider::rotarySliderFillColourId);
            auto centre = juce::Point<float>(x + width * 0.5f, y + height * 0.5f);
            const float diameter = std::min(40.0f, static_cast<float>(std::min(width, height)));
            auto body = juce::Rectangle<float>(diameter, diameter).withCentre(centre);
            g.setColour(juce::Colour::fromRGB(28, 28, 28));
            g.fillRoundedRectangle(body, 4.0f);
            g.setColour(colour.withAlpha(0.45f));
            g.drawRoundedRectangle(body, 4.0f, 1.0f);
            auto screw = body.reduced(7.0f);
            g.setGradientFill(juce::ColourGradient(colour.brighter(0.15f), screw.getTopLeft(),
                                                 colour.darker(0.5f), screw.getBottomRight(), false));
            g.fillEllipse(screw);
            g.setColour(juce::Colour::fromRGB(35, 32, 29));
            g.drawEllipse(screw, 1.0f);
            juce::Graphics::ScopedSaveState state(g);
            g.addTransform(juce::AffineTransform::rotation(startAngle + position * (endAngle - startAngle),
                                                         centre.x, centre.y));
            g.setColour(juce::Colour::fromRGB(30, 27, 24));
            g.fillRoundedRectangle(centre.x - 2.0f, centre.y - 9.0f, 4.0f, 18.0f, 1.0f);
            g.setColour(colour.brighter(0.35f));
            g.fillEllipse(centre.x - 1.5f, centre.y - 11.0f, 3.0f, 3.0f);
        }
    };

    class LogoButton : public juce::Button {
      public:
        LogoButton() : juce::Button("Power") {
            setClickingTogglesState(true);
            setMouseCursor(juce::MouseCursor::PointingHandCursor);
        }

        void paintButton(juce::Graphics& g, bool, bool) override {
            if (!mask.isValid() || getWidth() <= 32 || getHeight() <= 32)
                return;
            if (!cachedLogo.isValid()) {
                // Cache at Retina resolution with space around the logo for its halo.
                cachedLogo = juce::Image(juce::Image::ARGB, getWidth() * 2, getHeight() * 2, true);
                juce::Graphics logoGraphics(cachedLogo);
                logoGraphics.setColour(colour);
                logoGraphics.drawImage(mask, cachedLogo.getBounds().reduced(32).toFloat(),
                                       juce::RectanglePlacement::stretchToFit, true);
                cachedGlow = juce::Image(juce::Image::ARGB, cachedLogo.getWidth(), cachedLogo.getHeight(), true);
                juce::Graphics glowGraphics(cachedGlow);
                // Broad emission plus a brighter inner halo, like the active sliders.
                juce::DropShadow(colour.withMultipliedAlpha(0.85f), 24, {0, 0})
                    .drawForImage(glowGraphics, cachedLogo);
                juce::DropShadow(colour.withMultipliedAlpha(0.65f), 12, {0, 0})
                    .drawForImage(glowGraphics, cachedLogo);
            }
            juce::Graphics::ScopedSaveState state(g);
            g.setOpacity(powerAmount);
            g.drawImage(cachedGlow, getLocalBounds().toFloat());
            g.setOpacity(0.35f + 0.65f * powerAmount);
            g.drawImage(cachedLogo, getLocalBounds().toFloat());
        }

        void setPowerAmount(float amount) {
            powerAmount = amount;
            repaint();
        }

        void resized() override {
            cachedLogo = {};
            cachedGlow = {};
        }

        juce::Image mask;
        juce::Colour colour;

      private:
        juce::Image cachedLogo, cachedGlow;
        float powerAmount = 1.0f;
    };

    void regenerateBackgroundTexture(int width, int height);
    void loadLogoImage();
    void setPowerState(bool enabled);
    void applyPowerVisuals(float amount);
    void timerCallback() override;
    void toggleDevelopmentPanel();
    bool powerInitialised = false;
    float powerAmount = 1.0f;
    float powerStartAmount = 1.0f;
    float powerTargetAmount = 1.0f;
    double powerStartTime = 0.0;

    // We need a reference to the processor object in order to access its parameters
    HexaAmpAudioProcessor& audioProcessor;

    // Polygon parameter panel
    jnsc::juce_interface::PolygonPanel polygonPanel;

    juce::Image cachedBackground;
    juce::Image logoImage;
    LogoButton logoButton;
    std::unique_ptr<juce::ParameterAttachment> bypassAttachment;
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
