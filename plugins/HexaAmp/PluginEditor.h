#pragma once
#include "Version.h"

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HexaAmpAudioProcessorEditor)
};
