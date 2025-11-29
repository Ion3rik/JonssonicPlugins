// Jonssonic Plugin Framework
// Automatic ControlPanel for APVTS parameters
// SPDX-License-Identifier: MIT

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "ControlPanelConfig.h"
#include <parameters/ParameterIdUtils.h>

namespace Jonssonic {

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
    ControlPanel(juce::AudioProcessorValueTreeState& apvts, const ControlPanelConfig& config)
        : apvts(apvts), config(config)
    {
        buildControls();
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(config.backgroundColor);
    }

    void resized() override {
        layoutControls();
    }

    // Access to created controls if needed
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
            if (!paramWithID) continue;
            juce::String paramID = paramWithID->paramID;

            std::unique_ptr<juce::Component> control;

            if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param)) {
                DBG("[ControlPanel] Attaching Slider to paramID: " + paramID);
                auto* slider = new juce::Slider();
                slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
                slider->setTextBoxStyle(config.showValueBoxes ? juce::Slider::TextBoxBelow : juce::Slider::NoTextBox, false, 60, 20);
                slider->setTextValueSuffix(" " + floatParam->getLabel());
                slider->setRange(floatParam->range.start, floatParam->range.end);
                sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramID, *slider));
                control.reset(slider);
            } else if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param)) {
                DBG("[ControlPanel] Attaching ComboBox to paramID: " + paramID);
                auto* combo = new juce::ComboBox();
                for (int i = 0; i < choiceParam->choices.size(); ++i)
                    combo->addItem(choiceParam->choices[i], i + 1);
                comboBoxAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, paramID, *combo));
                control.reset(combo);
            } else if (auto* boolParam = dynamic_cast<juce::AudioParameterBool*>(param)) {
                DBG("[ControlPanel] Attaching Button to paramID: " + paramID);
                auto* button = new juce::ToggleButton();
                buttonAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, paramID, *button));
                control.reset(button);
            } else {
                DBG("[ControlPanel] Skipping unknown param type for paramID: " + paramID);
                continue;
            }

            if (control) {
                auto* label = new juce::Label();
                label->setText(param->getName(64), juce::dontSendNotification);
                label->setJustificationType(juce::Justification::centred);
                // Attach for Left (true) or Above (false), never for Below/Right
                if (config.labelPosition == ControlPanelConfig::LabelPosition::Left)
                    label->attachToComponent(control.get(), true);
                else if (config.labelPosition == ControlPanelConfig::LabelPosition::Above)
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
        int x = config.spacing;
        int y = config.spacing;
        int col = 0;
        int width = (getWidth() - (config.columns + 1) * config.spacing) / config.columns;
        int controlH = config.controlHeight;
        int labelH = config.labelHeight;

        for (size_t i = 0; i < ownedControls.size(); ++i) {
            auto* control = ownedControls[i].get();
            auto* label = labels[i];
            int cx = x + col * (width + config.spacing);
            int cy = y;

            if (config.labelPosition == ControlPanelConfig::LabelPosition::Above) {
                // Label is attached, only set control bounds
                control->setBounds(cx, cy + labelH, width, controlH);
            } else if (config.labelPosition == ControlPanelConfig::LabelPosition::Below) {
                control->setBounds(cx, cy, width, controlH);
                label->setBounds(cx, cy + controlH, width, labelH);
            } else if (config.labelPosition == ControlPanelConfig::LabelPosition::Left) {
                // Label is attached, only set control bounds
                control->setBounds(cx + labelH, cy, width - labelH, controlH);
            } else if (config.labelPosition == ControlPanelConfig::LabelPosition::Right) {
                control->setBounds(cx, cy, width - labelH, controlH);
                label->setBounds(cx + width - labelH, cy, labelH, controlH);
            }

            if (++col >= config.columns) {
                col = 0;
                y += controlH + labelH + config.spacing;
            }
        }
    }

    // Attachments must be kept alive
};

} // namespace Jonssonic
