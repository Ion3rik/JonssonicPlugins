// Jonssonic Plugin Framework
// Automatic ControlPanel for APVTS parameters
// SPDX-License-Identifier: MIT

#pragma once
#include "ControlPanelConfig.h"
#include "Version.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <parameters/ParameterIdUtils.h>

namespace jnsc::juce_interface {

/**
 * @brief Automatic parameter control panel for JUCE APVTS
 *
 * Usage:
 *   ControlPanelConfig config;
 *   config.columns = 3;
 *   config.showValueBoxes = true;
 *   config.controlHeight = 100;
 *   ControlPanel panel(apvts, config);
 *   addAndMakeVisible(panel);
 *
 * Features:
 * - Automatic control creation for all APVTS parameters
 * - Grid layout (configurable columns, spacing, etc.)
 * - Supports Slider, ComboBox, Button (auto-detected)
 * - Custom LookAndFeel and control overrides supported
 */
class ControlPanel : public juce::Component {
  public:
    /**
     * @brief Constructor
     * @param apvts Reference to the AudioProcessorValueTreeState
     * @param config Configuration struct for panel layout settings
     */
    ControlPanel(juce::AudioProcessorValueTreeState& apvts, const ControlPanelConfig& config)
        : apvts(apvts), config(config) {
        buildControls();
    }

    /**
     * @brief Paint the control panel.
     * @param g Graphics context
     */
    void paint(juce::Graphics& g) override {
        g.fillAll(config.backgroundColor);

        // Draw title if set
        if (config.title.isNotEmpty()) {
            g.setColour(juce::Colours::white);
            juce::Font titleFont(juce::FontOptions(config.fontName, config.titleHeight, juce::Font::bold));
            g.setFont(titleFont);

            int justificationFlags = 0;
            switch (config.titleHorizontalPlacement) {
            case ControlPanelConfig::HorizontalPlacement::Left:
                justificationFlags = juce::Justification::left;
                break;
            case ControlPanelConfig::HorizontalPlacement::Center:
                justificationFlags = juce::Justification::centred;
                break;
            case ControlPanelConfig::HorizontalPlacement::Right:
                justificationFlags = juce::Justification::right;
                break;
            }
            int titleY = 0;
            switch (config.titleVerticalPlacement) {
            case ControlPanelConfig::VerticalPlacement::Top:
                titleY = config.titleMarginY;
                break;
            case ControlPanelConfig::VerticalPlacement::Center:
                titleY = (getHeight() - config.titleHeight) / 2 + config.titleMarginY;
                break;
            case ControlPanelConfig::VerticalPlacement::Bottom:
                titleY = getHeight() - config.titleHeight - config.titleMarginY;
                break;
            }
            g.drawText(config.title,
                       config.titleMarginX,
                       titleY,
                       getWidth() - config.titleMarginX * 2,
                       config.titleHeight,
                       justificationFlags);

            // Draw version string in top right corner
            constexpr int margin = 8;
            juce::String versionStr = "v" JONSSONIC_PLUGINS_VERSION_STRING;
            g.setColour(juce::Colours::lightgrey.withAlpha(0.7f));
            g.setFont(juce::Font(juce::FontOptions(config.fontName, 14.0f, juce::Font::plain)));
            juce::GlyphArrangement glyphs;
            glyphs.addLineOfText(g.getCurrentFont(), versionStr, 0.0f, 0.0f);
            float textWidth = glyphs.getBoundingBox(0, glyphs.getNumGlyphs(), true).getWidth();
            int textHeight = (int)g.getCurrentFont().getHeight();
            g.drawText(versionStr,
                       getWidth() - (int)textWidth - margin,
                       margin,
                       (int)textWidth,
                       textHeight,
                       juce::Justification::topRight,
                       false);
        }

        // Draw subtitle if set
        if (config.subtitle.isNotEmpty()) {
            g.setColour(juce::Colours::white.withAlpha(1.0f));
            juce::Font subtitleFont(juce::FontOptions(config.fontName, config.subtitleHeight, juce::Font::plain));
            g.setFont(subtitleFont);

            int justificationFlags = 0;
            switch (config.subtitleHorizontalPlacement) {
            case ControlPanelConfig::HorizontalPlacement::Left:
                justificationFlags = juce::Justification::left;
                break;
            case ControlPanelConfig::HorizontalPlacement::Center:
                justificationFlags = juce::Justification::centred;
                break;
            case ControlPanelConfig::HorizontalPlacement::Right:
                justificationFlags = juce::Justification::right;
                break;
            }
            // Always top-align vertically
            justificationFlags |= juce::Justification::top;
            juce::Justification justification(justificationFlags);

            int subtitleY = config.subtitleMarginY;
            switch (config.subtitleVerticalPlacement) {
            case ControlPanelConfig::VerticalPlacement::Top:
                subtitleY += config.subtitleMarginY;
                if (config.title.isNotEmpty() &&
                    config.titleVerticalPlacement == ControlPanelConfig::VerticalPlacement::Top)
                    subtitleY += config.titleHeight;
                break;
            case ControlPanelConfig::VerticalPlacement::Center:
                subtitleY = (getHeight() - config.subtitleHeight) / 2 + config.subtitleMarginY;
                break;
            case ControlPanelConfig::VerticalPlacement::Bottom:
                subtitleY = getHeight() - config.subtitleHeight - config.subtitleMarginY;
                break;
            }

            g.drawText(config.subtitle,
                       config.subtitleMarginX,
                       subtitleY,
                       getWidth() - config.subtitleMarginX * 2,
                       config.subtitleHeight,
                       justification);
        }
    }

    /// Resize calls @ref layoutControls.
    void resized() override { layoutControls(); }

    /// Access to created controls if needed
    juce::Component* getControlForParam(const juce::String& paramID) {
        auto it = controls.find(paramID);
        return it != controls.end() ? it->second : nullptr;
    }

  private:
    juce::AudioProcessorValueTreeState& apvts;
    ControlPanelConfig config;
    std::vector<std::unique_ptr<juce::Component>> ownedControls;
    std::unordered_map<juce::String, juce::Component*> controls;
    std::vector<juce::Label*> labels;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> comboBoxAttachments;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments;

    void buildControls() {
        ownedControls.clear();
        controls.clear();
        labels.clear();
        sliderAttachments.clear();
        comboBoxAttachments.clear();
        buttonAttachments.clear();
        removeAllChildren();

        auto& params = apvts.processor.getParameters();
        for (auto* param : params) {
            auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(param);
            if (!paramWithID)
                continue;
            juce::String paramID = paramWithID->paramID;

            std::unique_ptr<juce::Component> control;

            if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param)) {
                DBG("[ControlPanel] Attaching Slider to paramID: " + paramID);
                auto* slider = new juce::Slider();
                slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
                slider->setTextBoxStyle(config.showValueBoxes ? juce::Slider::TextBoxBelow : juce::Slider::NoTextBox,
                                        false,
                                        60,
                                        20);
                slider->setTextValueSuffix(" " + floatParam->getLabel());
                slider->setRange(floatParam->range.start, floatParam->range.end);
                sliderAttachments.emplace_back(
                    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramID, *slider));
                control.reset(slider);
            }

            else if (auto* intParam = dynamic_cast<juce::AudioParameterInt*>(param)) {
                DBG("[ControlPanel] Attaching Slider to paramID: " + paramID);
                auto* slider = new juce::Slider();
                slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
                slider->setTextBoxStyle(config.showValueBoxes ? juce::Slider::TextBoxBelow : juce::Slider::NoTextBox,
                                        false,
                                        60,
                                        20);
                slider->setTextValueSuffix(" " + intParam->getLabel());
                slider->setRange(intParam->getNormalisableRange().start, intParam->getNormalisableRange().end, 1);
                sliderAttachments.emplace_back(
                    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramID, *slider));
                control.reset(slider);
            } else if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param)) {
                DBG("[ControlPanel] Attaching ComboBox to paramID: " + paramID);
                auto* combo = new juce::ComboBox();
                for (int i = 0; i < choiceParam->choices.size(); ++i)
                    combo->addItem(choiceParam->choices[i], i + 1);
                comboBoxAttachments.emplace_back(
                    std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, paramID, *combo));
                control.reset(combo);
            } else if (auto* boolParam = dynamic_cast<juce::AudioParameterBool*>(param)) {
                DBG("[ControlPanel] Attaching Button to paramID: " + paramID);
                auto* button = new juce::ToggleButton();
                buttonAttachments.emplace_back(
                    std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, paramID, *button));
                control.reset(button);
            } else {
                DBG("[ControlPanel] Skipping unknown param type for paramID: " + paramID);
                continue;
            }

            if (control) {
                auto* label = new juce::Label();
                label->setText(param->getName(64), juce::dontSendNotification);
                label->setJustificationType(juce::Justification::centred);
                label->setFont(juce::Font(juce::FontOptions(config.fontName, 14.0f, config.fontStyle)));
                // Attach label to control for accessibility, but layout is handled manually
                label->attachToComponent(control.get(), false);
                labels.push_back(label);
                addAndMakeVisible(label);

                addAndMakeVisible(control.get());
                controls[paramID] = control.get();
                ownedControls.push_back(std::move(control));
            }
        }
    }

    void layoutControls() {

        // Add panel margins
        int x = config.panelMarginLeft + config.spacing;
        int y = config.panelMarginTop + config.spacing;
        int availableWidth = getWidth() - config.panelMarginLeft - config.panelMarginRight;
        int availableHeight = getHeight() - config.panelMarginTop - config.panelMarginBottom;

        // Add top margin before title if present
        if (config.title.isNotEmpty() && config.titleVerticalPlacement == ControlPanelConfig::VerticalPlacement::Top) {
            y += config.titleMarginY; // top margin before title
            y += config.titleHeight;
            y += config.titleMarginY; // bottom margin after title
        }
        // Add top margin before subtitle if present
        if (config.subtitle.isNotEmpty() &&
            config.subtitleVerticalPlacement == ControlPanelConfig::VerticalPlacement::Top) {
            y += config.subtitleMarginY; // top margin before subtitle
            y += config.subtitleHeight;
            y += config.subtitleMarginY; // bottom margin after subtitle
        }
        int col = 0;
        int width = (availableWidth - (config.columns + 1) * config.spacing) / config.columns;
        int controlH = config.controlHeight;
        int labelH = config.labelHeight;
        const int valueBoxHeight = config.showValueBoxes ? 20 : 0;

        for (size_t i = 0; i < ownedControls.size(); ++i) {
            auto* control = ownedControls[i].get();
            auto* label = labels[i];
            int cx = x + col * (width + config.spacing);
            int cy = y;

            label->setFont(juce::Font(juce::FontOptions(config.fontName, (float)labelH, config.fontStyle)));

            // For toggle buttons, place checkbox left and label right in the same row
            if (dynamic_cast<juce::ToggleButton*>(control)) {
                int toggleW = 24;                              // fixed checkbox width
                int toggleH = 24;                              // fixed checkbox height
                int labelW = width - toggleW - 8;              // 8px gap between checkbox and label
                int toggleX = cx + (width - toggleW - labelW); // center the pair
                int labelX = toggleX + toggleW + 8;
                int rowH = std::max(labelH, toggleH);
                int rowY = cy; // position below where the label would be (for consistency with sliders)
                control->setBounds(toggleX, rowY, toggleW, toggleH);
                label->setBounds(labelX, rowY, labelW, rowH);
                label->setJustificationType(juce::Justification::centredLeft);

            } else {
                // For all other controls, label above
                label->setBounds(cx, cy, width, labelH);
                control->setBounds(cx, cy + labelH, width, controlH);
                cy += labelH + controlH + valueBoxHeight;
            }

            if (++col >= config.columns) {
                col = 0;
                y += (labelH + controlH + valueBoxHeight + config.spacing);
            }
        }
    }
};

} // namespace jnsc::juce_interface
