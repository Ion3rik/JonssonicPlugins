#pragma once
#include <array>

#include "PluginProcessor.h"
#include "HexaGuitarFonts.h"
#include <MinimalJuceHeader.h>
#include <gui/polygon_gui/PolygonPanel.h>
#include <parameters/ParameterIdUtils.h>

class HexaGuitarAudioProcessorEditor : public juce::AudioProcessorEditor {
  public:
    HexaGuitarAudioProcessorEditor(HexaGuitarAudioProcessor&);
    ~HexaGuitarAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    static const std::array<juce::Colour, 12>& paletteColors() {
        static const std::array<juce::Colour, 12> colors{juce::Colours::white,
                juce::Colour(0xffe08079), juce::Colour(0xffe79d6d), juce::Colour(0xffe1b45a),
                juce::Colour(0xffe0cf67), juce::Colour(0xffc4d16d), juce::Colour(0xff91c17b),
                juce::Colour(0xff62b99a), juce::Colour(0xff6cbec8), juce::Colour(0xff74abda),
                juce::Colour(0xff8996d6), juce::Colour(0xffaa82cf)};
        return colors;
    }

    class ThemeButton : public juce::Button {
      public:
        ThemeButton() : juce::Button("Switch light/dark mode") {
            setMouseCursor(juce::MouseCursor::PointingHandCursor);
        }
        void setPaletteColour(juce::Colour value) { lightColour = value; repaint(); }
        void setDark(bool value) { dark = value; repaint(); }
        void paintButton(juce::Graphics& g, bool, bool) override {
            const auto circle = getLocalBounds().toFloat().reduced(5.0f);
            juce::Path mask;
            mask.addEllipse(circle);
            {
                juce::Graphics::ScopedSaveState state(g);
                g.reduceClipRegion(mask);
                g.setColour(dark ? juce::Colours::black : lightColour);
                g.fillRect(circle);
                g.setColour(dark ? lightColour : juce::Colours::black);
                g.fillRect(circle.withLeft(circle.getCentreX()));
            }
            g.setColour(dark ? lightColour : juce::Colours::black);
            g.drawEllipse(circle, 1.0f);
        }
      private:
        bool dark = true;
        juce::Colour lightColour = juce::Colours::white;
    };

    class PaletteButton : public juce::Button {
      public:
        PaletteButton() : juce::Button("Choose palette color") {
            setMouseCursor(juce::MouseCursor::PointingHandCursor);
        }
        std::function<void()> onPopup;
        void mouseDown(const juce::MouseEvent& event) override {
            popupPress = event.mods.isPopupMenu();
            if (popupPress) {
                if (onPopup)
                    onPopup();
            } else {
                juce::Button::mouseDown(event);
            }
        }
        void mouseUp(const juce::MouseEvent& event) override {
            if (popupPress) {
                popupPress = false;
                return;
            }
            juce::Button::mouseUp(event);
        }
        bool popupPress = false;
        juce::Colour ink = juce::Colours::white;
        void paintButton(juce::Graphics& g, bool, bool) override {
            auto area = getLocalBounds().toFloat().reduced(6.0f);
            juce::Path palette;
            palette.startNewSubPath(area.getRight(), area.getCentreY());
            palette.cubicTo(area.getRight(), area.getY() - 4.0f, area.getX(), area.getY() - 4.0f,
                            area.getX(), area.getCentreY());
            palette.cubicTo(area.getX(), area.getBottom() + 2.0f, area.getRight(), area.getBottom() + 2.0f,
                            area.getCentreX(), area.getCentreY() + 3.0f);
            palette.closeSubPath();
            g.setColour(ink);
            g.strokePath(palette, juce::PathStrokeType(1.2f));
            for (auto point : {juce::Point<float>(0.25f, 0.35f), {0.5f, 0.22f}, {0.75f, 0.35f}})
                g.fillEllipse(area.getX() + point.x * area.getWidth() - 1.5f,
                              area.getY() + point.y * area.getHeight() - 1.5f, 3.0f, 3.0f);
        }
    };

    class DiceButton : public juce::Button {
      public:
        DiceButton() : juce::Button("Randomize controls except Level") {
            setMouseCursor(juce::MouseCursor::PointingHandCursor);
        }
        juce::Colour ink = juce::Colours::white;
        void paintButton(juce::Graphics& g, bool, bool) override {
            const auto area = getLocalBounds().toFloat().reduced(6.0f);
            g.setColour(ink);
            g.drawRoundedRectangle(area, 3.0f, 1.2f);
            for (auto point : {juce::Point<float>(0.28f, 0.25f), {0.72f, 0.25f},
                               {0.28f, 0.5f}, {0.72f, 0.5f}, {0.28f, 0.75f}, {0.72f, 0.75f}})
                g.fillEllipse(area.getX() + point.x * area.getWidth() - 1.3f,
                              area.getY() + point.y * area.getHeight() - 1.3f, 2.6f, 2.6f);
        }
    };

    class PaletteChoices : public juce::Component {
      public:
        PaletteChoices(juce::Colour selected, juce::Colour foreground, juce::Colour background,
                       std::function<void(juce::Colour)> choose) : ink(foreground), paper(background) {
            const auto& colors = paletteColors();
            const juce::StringArray names{"White", "Ember", "Apricot", "Ochre", "Straw", "Lichen",
                                          "Sage", "Sea glass", "Fjord", "Sky", "Indigo", "Heather"};
            for (size_t i = 0; i < colors.size(); ++i) {
                auto button = std::make_unique<Swatch>(colors[i], ink, paper, colors[i] == selected);
                button->setName(names[static_cast<int>(i)]);
                button->onClick = [this, choose, color = colors[i]] {
                    choose(color);
                    if (auto* popup = findParentComponentOfClass<juce::CallOutBox>())
                        popup->dismiss();
                };
                button->setBounds(8 + static_cast<int>(i % 6) * 32, 8 + static_cast<int>(i / 6) * 32, 32, 32);
                addAndMakeVisible(*button);
                buttons.push_back(std::move(button));
            }
            setSize(208, 80);
        }
        void paint(juce::Graphics& g) override {
            g.fillAll(paper);
        }
      private:
        class Swatch : public juce::Button {
          public:
            Swatch(juce::Colour value, juce::Colour outline, juce::Colour background, bool active)
                : juce::Button("Color"), color(value), ink(outline), paper(background), selected(active) {}
            void paintButton(juce::Graphics& g, bool, bool) override {
                const auto circle = getLocalBounds().toFloat().reduced(6.0f);
                g.setColour(color);
                g.fillEllipse(circle);
                g.setColour(ink);
                if (color == paper)
                    g.drawEllipse(circle, 1.0f);
                if (selected)
                    g.drawEllipse(circle.expanded(3.0f), 1.0f);
            }
          private:
            juce::Colour color, ink, paper;
            bool selected;
        };
        juce::Colour ink, paper;
        std::vector<std::unique_ptr<Swatch>> buttons;
    };

    class VersionHotspot : public juce::Component {
      public:
        explicit VersionHotspot(HexaGuitarAudioProcessorEditor& editor) : editor(editor) {}
        void mouseDoubleClick(const juce::MouseEvent&) override { editor.toggleDevelopmentPanel(); }

      private:
        HexaGuitarAudioProcessorEditor& editor;
    };

    class TrimmerLookAndFeel : public juce::LookAndFeel_V4 {
      public:
        juce::Colour popupInk = juce::Colours::white, popupPaper = juce::Colours::black;
        void drawCallOutBoxBackground(juce::CallOutBox&, juce::Graphics& g,
                                      const juce::Path& path, juce::Image&) override {
            g.setColour(popupPaper);
            g.fillPath(path);
            g.setColour(popupInk);
            g.strokePath(path, juce::PathStrokeType(1.0f));
        }

        juce::Font getLabelFont(juce::Label&) override {
            return HexaGuitarFonts::font(14.0f);
        }

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
            if (drawable == nullptr || getWidth() <= 32 || getHeight() <= 32)
                return;
            if (colour != drawableColour) {
                drawable->replaceColour(drawableColour, colour);
                drawableColour = colour;
            }
            drawable->drawWithin(g, getLocalBounds().toFloat().withSizeKeepingCentre(42.0f, 42.0f),
                                 juce::RectanglePlacement::centred, 1.0f);
        }

        std::unique_ptr<juce::Drawable> drawable;
        juce::Colour drawableColour = juce::Colours::black;
        juce::Colour colour;

    };

    void applyMonochrome();
    bool darkMode = true;
    juce::Colour selectedColour = juce::Colours::white;
    PaletteButton paletteButton;
    DiceButton diceButton;
    juce::Component::SafePointer<juce::CallOutBox> palettePopup;
    ThemeButton themeToggle;
    void loadLogo();
    void setPowerState(bool enabled);
    void toggleDevelopmentPanel();

    // We need a reference to the processor object in order to access its parameters
    HexaGuitarAudioProcessor& audioProcessor;

    // Polygon parameter panel
    jnsc::juce_interface::PolygonPanel polygonPanel;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HexaGuitarAudioProcessorEditor)
};
