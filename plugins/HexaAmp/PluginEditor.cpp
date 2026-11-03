#include "PluginEditor.h"
#include <utils/ResourceUtils.h>

HexaAmpAudioProcessorEditor::HexaAmpAudioProcessorEditor(HexaAmpAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p),
      polygonPanel(audioProcessor.getAPVTS(),
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
    logoButton.mask = logoImage;
    logoButton.colour = polygonPanel.getCentralDisplay().getBrandColour();
    addAndMakeVisible(logoButton);
    auto* bypass = audioProcessor.getAPVTS().getParameter(jnsc::juce_interface::idToString(HexaAmpParams::ID::Bypass));
    bypassAttachment = std::make_unique<juce::ParameterAttachment>(
        *bypass,
        [this, bypass](float value) {
            const bool enabled = value < 0.5f;
            logoButton.setToggleState(enabled, juce::dontSendNotification);
            const auto tooltip = jnsc::juce_interface::getParameterTooltip(*bypass);
            logoButton.setTooltip(tooltip.isNotEmpty() ? tooltip : (enabled ? "Turn effect off" : "Turn effect on"));
            setPowerState(enabled);
        },
        nullptr);
    logoButton.onClick = [this] {
        bypassAttachment->setValueAsCompleteGesture(logoButton.getToggleState() ? 0.0f : 1.0f);
    };
    bypassAttachment->sendInitialUpdate();

    addChildComponent(developmentViewport);
    developmentViewport.setViewedComponent(&developmentPanel, false);
    developmentViewport.setScrollBarsShown(true, false);
    developmentVersion.setText("v0.0.1", juce::dontSendNotification);
    developmentVersion.setFont(juce::Font(juce::FontOptions(12.0f)));
    developmentVersion.setJustificationType(juce::Justification::centredRight);
    developmentVersion.setBorderSize(juce::BorderSize<int>(0));
    addChildComponent(developmentVersion);
    const auto developmentColour = polygonPanel.getCentralDisplay().getBrandColour();
    developmentVersion.setColour(juce::Label::textColourId, developmentColour.withAlpha(0.6f));
    using ID = HexaAmpParams::ID;
    const std::array<ID, 11> trimIDs{
        ID::PreAmpCompressorAttack, ID::PreAmpCompressorRelease, ID::PreAmpCompressorThresholdMin, ID::PreAmpCompressorThresholdMax,
        ID::PowerAmpCompressorAttack, ID::PowerAmpCompressorRelease, ID::PowerAmpCompressorThresholdMin, ID::PowerAmpCompressorThresholdMax, ID::Makeup,
        ID::MidBoostFrequency, ID::PowerAmpShape};
    for (size_t i = 0; i < trimIDs.size(); ++i) {
        const auto paramID = jnsc::juce_interface::idToString(trimIDs[i]);
        auto* parameter = audioProcessor.getAPVTS().getParameter(paramID);
        auto& label = developmentLabels[i];
        auto& slider = developmentSliders[i];
        label.setText(parameter->getName(64), juce::dontSendNotification);
        label.setFont(juce::Font(juce::FontOptions(13.0f)));
        label.setJustificationType(juce::Justification::centredLeft);
        label.setColour(juce::Label::textColourId, developmentColour);
        developmentPanel.addAndMakeVisible(label);
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setLookAndFeel(&trimmerLookAndFeel);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 110, 24);
        slider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                                   juce::MathConstants<float>::pi * 2.75f, true);
        const auto unit = parameter->getLabel();
        slider.setNumDecimalPlacesToDisplay(unit == "ms" || unit == "dB" || trimIDs[i] == ID::PowerAmpShape ? 1 : 0);
        slider.setTextValueSuffix(unit.isEmpty() ? juce::String{} : " " + unit);
        slider.setDoubleClickReturnValue(true, parameter->convertFrom0to1(parameter->getDefaultValue()));
        slider.setMouseDragSensitivity(400);
        slider.setColour(juce::Slider::rotarySliderFillColourId, developmentColour);
        slider.setColour(juce::Slider::textBoxTextColourId, developmentColour);
        slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB(12, 16, 14));
        slider.setColour(juce::Slider::textBoxOutlineColourId, developmentColour.withAlpha(0.2f));
        const auto tooltip = jnsc::juce_interface::getParameterTooltip(*parameter);
        slider.setTooltip(tooltip);
        label.setTooltip(tooltip);
        developmentPanel.addAndMakeVisible(slider);
        developmentAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), paramID, slider);
    }
    addAndMakeVisible(versionHotspot);
    resized();
}

HexaAmpAudioProcessorEditor::~HexaAmpAudioProcessorEditor() {
    for (auto& slider : developmentSliders)
        slider.setLookAndFeel(nullptr);
}

void HexaAmpAudioProcessorEditor::applyPowerVisuals(float amount) {
    powerAmount = amount;
    polygonPanel.setPowerVisualAmount(amount);
    logoButton.setPowerAmount(amount);
}

void HexaAmpAudioProcessorEditor::setPowerState(bool enabled) {
    polygonPanel.setProcessingEnabled(enabled);
    const float target = enabled ? 1.0f : 0.0f;
    if (!powerInitialised || !isShowing()) {
        powerInitialised = true;
        powerTargetAmount = target;
        stopTimer();
        applyPowerVisuals(target);
        return;
    }
    if (powerTargetAmount == target)
        return;
    powerTargetAmount = target;
    powerStartAmount = powerAmount;
    powerStartTime = juce::Time::getMillisecondCounterHiRes();
    startTimerHz(60);
}

void HexaAmpAudioProcessorEditor::timerCallback() {
    const float progress =
        juce::jlimit(0.0f,
                     1.0f,
                     static_cast<float>((juce::Time::getMillisecondCounterHiRes() - powerStartTime) / 220.0));
    if (progress >= 1.0f || !isShowing()) {
        stopTimer();
        applyPowerVisuals(powerTargetAmount);
        return;
    }
    const float eased = progress * progress * (3.0f - 2.0f * progress);
    applyPowerVisuals(powerStartAmount + (powerTargetAmount - powerStartAmount) * eased);
}

//==============================================================================
void HexaAmpAudioProcessorEditor::paint(juce::Graphics& g) {
    if (!cachedBackground.isValid())
        regenerateBackgroundTexture(getWidth(), getHeight());

    if (cachedBackground.isValid()) {
        g.drawImageAt(cachedBackground, 0, 0);
        return;
    }

    g.fillAll(juce::Colour::fromRGB(12, 16, 14));
}

void HexaAmpAudioProcessorEditor::resized() {
    cachedBackground = {};
    polygonPanel.setBounds(getLocalBounds());
    logoButton.setBounds(0, 0, 86, 86); // 54px logo with 16px of halo space on each side.
    developmentViewport.setBounds(24, 52, getWidth() - 48, getHeight() - 68);
    developmentPanel.setSize(developmentViewport.getWidth() - developmentViewport.getScrollBarThickness(),
                             static_cast<int>(developmentSliders.size()) * 40);
    developmentVersion.setBounds(getWidth() - 112, 16, 96, 20);
    const int rowHeight = 40;
    const int panelWidth = developmentPanel.getWidth();
    for (size_t i = 0; i < developmentSliders.size(); ++i) {
        const int y = static_cast<int>(i) * rowHeight;
        developmentLabels[i].setBounds(8, y, panelWidth / 2 - 8, rowHeight);
        developmentSliders[i].setBounds(panelWidth / 2, y + 2, panelWidth / 2, rowHeight - 4);
    }
    versionHotspot.setBounds(getWidth() - 112, 16, 96, 20);
}

void HexaAmpAudioProcessorEditor::toggleDevelopmentPanel() {
    developmentVisible = !developmentVisible;
    polygonPanel.setVisible(!developmentVisible);
    logoButton.setVisible(!developmentVisible);
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

void HexaAmpAudioProcessorEditor::regenerateBackgroundTexture(int width, int height) {
    if (width <= 0 || height <= 0)
        return;

    cachedBackground = juce::Image(juce::Image::ARGB, width, height, true);
    juce::Graphics g(cachedBackground);

    const auto bounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

    const auto centerX = bounds.getCentreX();
    const auto centerY = bounds.getCentreY();
    juce::Colour center = juce::Colour::fromRGB(18, 27, 22);
    juce::Colour edge = juce::Colour::fromRGB(6, 9, 8);
    juce::ColourGradient bg(center, centerX, centerY, edge, bounds.getRight(), bounds.getBottom(), true);
    g.setGradientFill(bg);
    g.fillRect(bounds);

    // Matte micro-grain: small, subtle speckles instead of coarse grain.
    juce::Random rng(0x5EEDu);
    for (int i = 0; i < (width * height) / 3; ++i) {
        const float x = rng.nextFloat() * width;
        const float y = rng.nextFloat() * height;
        const float a = 0.004f + 0.010f * rng.nextFloat();
        const float w = 0.28f + 0.68f * rng.nextFloat();
        g.setColour(juce::Colours::white.withAlpha(a));
        g.fillEllipse(x, y, w, w);
    }

    // Counter-tone dark grain adds matte depth without visible chunky dots.
    for (int i = 0; i < (width * height) / 4; ++i) {
        const float x = rng.nextFloat() * width;
        const float y = rng.nextFloat() * height;
        const float a = 0.003f + 0.008f * rng.nextFloat();
        const float w = 0.22f + 0.62f * rng.nextFloat();
        g.setColour(juce::Colours::black.withAlpha(a));
        g.fillEllipse(x, y, w, w);
    }
}
