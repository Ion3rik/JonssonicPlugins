#include "PluginEditor.h"
#include <utils/ResourceUtils.h>

HexaAmpAudioProcessorEditor::HexaAmpAudioProcessorEditor(HexaAmpAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p),
      polygonPanel(
          audioProcessor.getAPVTS(),
          "Hexa-Amp",
          std::vector<juce::String>{jnsc::juce_interface::idToString(HexaAmpParams::ID::Drive),
                                    jnsc::juce_interface::idToString(HexaAmpParams::ID::Bite),
                                    jnsc::juce_interface::idToString(HexaAmpParams::ID::Squish),
                                    jnsc::juce_interface::idToString(HexaAmpParams::ID::Level),
                                    jnsc::juce_interface::idToString(HexaAmpParams::ID::Color),
                                    jnsc::juce_interface::idToString(HexaAmpParams::ID::Tilt)},
          "J O N S S O N I C",
          juce::StringArray{jnsc::juce_interface::idToString(HexaAmpParams::ID::Bypass),
                            jnsc::juce_interface::idToString(HexaAmpParams::ID::PreAmpCompressorAttack),
                            jnsc::juce_interface::idToString(HexaAmpParams::ID::PreAmpCompressorRelease),
                            jnsc::juce_interface::idToString(HexaAmpParams::ID::PowerAmpCompressorAttack),
                            jnsc::juce_interface::idToString(HexaAmpParams::ID::PowerAmpCompressorRelease),
                            jnsc::juce_interface::idToString(HexaAmpParams::ID::MidBoostFrequency),
                            jnsc::juce_interface::idToString(HexaAmpParams::ID::PowerAmpShape),
                            jnsc::juce_interface::idToString(HexaAmpParams::ID::PreAmpCompressorThresholdMin),
                            jnsc::juce_interface::idToString(HexaAmpParams::ID::PreAmpCompressorThresholdMax),
                            jnsc::juce_interface::idToString(HexaAmpParams::ID::PowerAmpCompressorThresholdMin),
                            jnsc::juce_interface::idToString(HexaAmpParams::ID::PowerAmpCompressorThresholdMax),
                            jnsc::juce_interface::idToString(HexaAmpParams::ID::Makeup)},
          "v0.0.1") {
    setOpaque(true);
    addAndMakeVisible(polygonPanel);
    setSize(512, 512);
    loadLogoImage();
    logoDisplay.mask = logoImage;
    logoDisplay.colour = polygonPanel.getCentralDisplay().getBrandColour();
    addAndMakeVisible(logoDisplay);
    auto* bypass = audioProcessor.getAPVTS().getParameter(jnsc::juce_interface::idToString(HexaAmpParams::ID::Bypass));
    bypassStateAttachment = std::make_unique<juce::ParameterAttachment>(
        *bypass,
        [this](float value) { setPowerState(value < 0.5f); },
        nullptr);
    bypassStateAttachment->sendInitialUpdate();

    addChildComponent(developmentViewport);
    developmentViewport.setViewedComponent(&developmentPanel, false);
    developmentViewport.setScrollBarsShown(true, false);
    developmentViewport.setScrollBarThickness(12);
    developmentViewport.getVerticalScrollBar().setLookAndFeel(&trimmerLookAndFeel);
    developmentVersion.setText("v0.0.1", juce::dontSendNotification);
    developmentVersion.setFont(juce::Font(juce::FontOptions("Avenir", 12.0f, juce::Font::plain)));
    developmentVersion.setJustificationType(juce::Justification::centredRight);
    developmentVersion.setBorderSize(juce::BorderSize<int>(0));
    addChildComponent(developmentVersion);
    const auto developmentColour = polygonPanel.getCentralDisplay().getBrandColour();
    developmentVersion.setColour(juce::Label::textColourId, developmentColour);
    using ID = HexaAmpParams::ID;
    const std::array<ID, 11> trimIDs{ID::PreAmpCompressorAttack,
                                     ID::PreAmpCompressorRelease,
                                     ID::PreAmpCompressorThresholdMin,
                                     ID::PreAmpCompressorThresholdMax,
                                     ID::PowerAmpCompressorAttack,
                                     ID::PowerAmpCompressorRelease,
                                     ID::PowerAmpCompressorThresholdMin,
                                     ID::PowerAmpCompressorThresholdMax,
                                     ID::Makeup,
                                     ID::MidBoostFrequency,
                                     ID::PowerAmpShape};
    for (size_t i = 0; i < trimIDs.size(); ++i) {
        const auto paramID = jnsc::juce_interface::idToString(trimIDs[i]);
        auto* parameter = audioProcessor.getAPVTS().getParameter(paramID);
        auto& label = developmentLabels[i];
        auto& slider = developmentSliders[i];
        label.setText(parameter->getName(64), juce::dontSendNotification);
        label.setFont(juce::Font(juce::FontOptions("Avenir", 13.0f, juce::Font::plain)));
        label.setJustificationType(juce::Justification::centredLeft);
        label.setColour(juce::Label::textColourId, developmentColour);
        developmentPanel.addAndMakeVisible(label);
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setLookAndFeel(&trimmerLookAndFeel);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 90, 24);
        slider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                                   juce::MathConstants<float>::pi * 2.75f,
                                   true);
        const auto unit = parameter->getLabel();
        slider.setNumDecimalPlacesToDisplay(unit == "ms" || unit == "dB" || trimIDs[i] == ID::PowerAmpShape ? 1 : 0);
        slider.setTextValueSuffix(unit.isEmpty() ? juce::String{} : " " + unit);
        slider.setDoubleClickReturnValue(true, parameter->convertFrom0to1(parameter->getDefaultValue()));
        slider.setMouseDragSensitivity(400);
        slider.setColour(juce::Slider::rotarySliderFillColourId, developmentColour);
        slider.setColour(juce::Slider::textBoxTextColourId, developmentColour);
        slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
        slider.setColour(juce::Slider::textBoxOutlineColourId, developmentColour);
        const auto tooltip = jnsc::juce_interface::getParameterTooltip(*parameter);
        slider.setTooltip(tooltip);
        label.setTooltip(tooltip);
        developmentPanel.addAndMakeVisible(slider);
        developmentAttachments[i] =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(),
                                                                                   paramID,
                                                                                   slider);
    }
    addAndMakeVisible(versionHotspot);
    addAndMakeVisible(diceButton);
    diceButton.onClick = [this] {
        using ID = HexaAmpParams::ID;
        for (auto id : {ID::Drive, ID::Bite, ID::Squish, ID::Color, ID::Tilt}) {
            auto* parameter = audioProcessor.getAPVTS().getParameter(jnsc::juce_interface::idToString(id));
            parameter->beginChangeGesture();
            parameter->setValueNotifyingHost(juce::Random::getSystemRandom().nextFloat());
            parameter->endChangeGesture();
        }
    };
    addAndMakeVisible(paletteButton);
    paletteButton.onClick = [this] {
        const auto& colors = paletteColors();
        size_t index = 0;
        for (size_t i = 0; i < colors.size(); ++i)
            if (colors[i] == selectedColour)
                index = i;
        selectedColour = colors[(index + 1) % colors.size()];
        applyMonochrome();
    };
    paletteButton.onPopup = [this] {
        auto safe = juce::Component::SafePointer<HexaAmpAudioProcessorEditor>(this);
        const auto ink = darkMode ? selectedColour : juce::Colours::black;
        const auto paper = darkMode ? juce::Colours::black : selectedColour;
        auto choices = std::make_unique<PaletteChoices>(selectedColour, ink, paper, [safe](juce::Colour color) {
            if (safe != nullptr) {
                safe->selectedColour = color;
                safe->applyMonochrome();
            }
        });
        trimmerLookAndFeel.popupInk = ink;
        trimmerLookAndFeel.popupPaper = paper;
        palettePopup = &juce::CallOutBox::launchAsynchronously(std::move(choices), paletteButton.getBounds(), this);
        palettePopup->setLookAndFeel(&trimmerLookAndFeel);
    };
    addAndMakeVisible(themeToggle);
    themeToggle.onClick = [this] {
        darkMode = !darkMode;
        applyMonochrome();
    };
    polygonPanel.addFooterHelp(diceButton,
                               "Randomize",
                               "Randomize Drive, Bite, Squish, Color and Tilt. Level stays fixed.");
    polygonPanel.addFooterHelp(paletteButton,
                               "Palette",
                               "Click to cycle colors. Right-click to open the color picker.");
    polygonPanel.addFooterHelp(themeToggle, "Dark/Light", "Toggle betweeen dark and light modes.");
    applyMonochrome();
    resized();
}

HexaAmpAudioProcessorEditor::~HexaAmpAudioProcessorEditor() {
    if (palettePopup != nullptr) {
        palettePopup->setLookAndFeel(nullptr);
        palettePopup->dismiss();
    }
    developmentViewport.getVerticalScrollBar().setLookAndFeel(nullptr);
    for (auto& slider : developmentSliders)
        slider.setLookAndFeel(nullptr);
}

void HexaAmpAudioProcessorEditor::setPowerState(bool enabled) {
    polygonPanel.setProcessingEnabled(enabled);
    polygonPanel.setPowerVisualAmount(enabled ? 1.0f : 0.0f);
}

//==============================================================================
void HexaAmpAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(darkMode ? juce::Colours::black : selectedColour);
}

void HexaAmpAudioProcessorEditor::applyMonochrome() {
    const auto ink = darkMode ? selectedColour : juce::Colours::black;
    const auto paper = darkMode ? juce::Colours::black : selectedColour;
    polygonPanel.setMonochrome(darkMode, selectedColour);
    logoDisplay.colour = ink;
    logoDisplay.repaint();
    developmentVersion.setColour(juce::Label::textColourId, ink);
    for (size_t i = 0; i < developmentSliders.size(); ++i) {
        developmentLabels[i].setColour(juce::Label::textColourId, ink);
        developmentSliders[i].setColour(juce::Slider::rotarySliderFillColourId, ink);
        developmentSliders[i].setColour(juce::Slider::textBoxTextColourId, ink);
        developmentSliders[i].setColour(juce::Slider::textBoxBackgroundColourId, paper);
        developmentSliders[i].setColour(juce::Slider::textBoxOutlineColourId, ink);
    }
    developmentViewport.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, ink);
    developmentViewport.getVerticalScrollBar().setColour(juce::ScrollBar::backgroundColourId, paper);
    diceButton.ink = ink;
    diceButton.repaint();
    paletteButton.ink = ink;
    paletteButton.repaint();
    themeToggle.setPaletteColour(selectedColour);
    themeToggle.setDark(darkMode);
    repaint();
}

void HexaAmpAudioProcessorEditor::resized() {
    diceButton.setBounds(getWidth() - 128, getHeight() - 48, 32, 32);
    paletteButton.setBounds(getWidth() - 88, getHeight() - 48, 32, 32);
    themeToggle.setBounds(getWidth() - 48, getHeight() - 48, 32, 32);
    themeToggle.toFront(false);
    polygonPanel.setBounds(getLocalBounds());
    logoDisplay.setBounds(0, 0, 86, 86); // 54px logo with 16px of padding on each side.
    developmentViewport.setBounds(24, 52, getWidth() - 48, getHeight() - 100);
    const int rowHeight = 40;
    developmentPanel.setSize(developmentViewport.getWidth() - developmentViewport.getScrollBarThickness() - 12,
                             static_cast<int>(developmentSliders.size()) * rowHeight);
    developmentVersion.setBounds(getWidth() - 112, 16, 96, 20);
    const int panelWidth = developmentPanel.getWidth();
    int labelWidth = 0;
    for (const auto& label : developmentLabels)
        labelWidth =
            juce::jmax(labelWidth, juce::GlyphArrangement::getStringWidthInt(label.getFont(), label.getText()));
    labelWidth = juce::jmin(labelWidth + 2, panelWidth - 198);
    const int groupX = 8;
    const int sliderX = groupX + labelWidth + 24;
    for (size_t i = 0; i < developmentSliders.size(); ++i) {
        const int y = static_cast<int>(i) * rowHeight;
        developmentLabels[i].setBorderSize(juce::BorderSize<int>(0));
        developmentLabels[i].setBounds(groupX, y, labelWidth, rowHeight);
        developmentSliders[i].setBounds(sliderX, y + (rowHeight - 36) / 2, 158, 36);
    }
    versionHotspot.setBounds(getWidth() - 112, 16, 96, 20);
}

void HexaAmpAudioProcessorEditor::toggleDevelopmentPanel() {
    developmentVisible = !developmentVisible;
    polygonPanel.setVisible(!developmentVisible);
    logoDisplay.setVisible(!developmentVisible);
    developmentViewport.setVisible(developmentVisible);
    developmentVersion.setVisible(developmentVisible);
    versionHotspot.toFront(false);
}

void HexaAmpAudioProcessorEditor::loadLogoImage() {
    auto logoFile = jnsc::juce_interface::getResourceFile("logos/Jonssonic_logo.png");
    if (logoFile.existsAsFile())
        logoImage = juce::ImageFileFormat::loadFrom(logoFile);

    if (logoImage.isValid()) {
        juce::Image mask(juce::Image::SingleChannel, logoImage.getWidth(), logoImage.getHeight(), true);
        for (int y = 0; y < logoImage.getHeight(); ++y) {
            for (int x = 0; x < logoImage.getWidth(); ++x) {
                const auto pixel = logoImage.getPixelAt(x, y);
                // Black is transparent; white is opaque. Preserve antialiased edges
                // and any transparency already present in the source image.
                mask.setPixelAt(x, y, juce::Colours::white.withAlpha(pixel.getBrightness() * pixel.getFloatAlpha()));
            }
        }
        // A circular opening rounds the square stroke ends without blurring
        // the mask or changing the width of the straight strokes.
        const int cornerRadius = juce::jmax(1, juce::roundToInt(mask.getWidth() * 0.05f));
        const auto roundMask = [cornerRadius](const juce::Image& source, bool erode) {
            juce::Image result(juce::Image::SingleChannel, source.getWidth(), source.getHeight(), true);
            for (int y = 0; y < source.getHeight(); ++y) {
                for (int x = 0; x < source.getWidth(); ++x) {
                    float alpha = erode ? 1.0f : 0.0f;
                    for (int dy = -cornerRadius; dy <= cornerRadius; ++dy) {
                        for (int dx = -cornerRadius; dx <= cornerRadius; ++dx) {
                            if (dx * dx + dy * dy > cornerRadius * cornerRadius)
                                continue;
                            const int sampleX = x + dx;
                            const int sampleY = y + dy;
                            const float sample = source.getBounds().contains(sampleX, sampleY)
                                                     ? source.getPixelAt(sampleX, sampleY).getFloatAlpha()
                                                     : 0.0f;
                            alpha = erode ? juce::jmin(alpha, sample) : juce::jmax(alpha, sample);
                        }
                    }
                    result.setPixelAt(x, y, juce::Colours::white.withAlpha(alpha));
                }
            }
            return result;
        };
        logoImage = roundMask(roundMask(mask, true), false);
    }
}
