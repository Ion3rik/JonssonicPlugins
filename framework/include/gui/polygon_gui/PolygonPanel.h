// Jonssonic Plugin Framework
// Polygon panel for APVTS parameters
// SPDX-License-Identifier: MIT

#pragma once

#include "PolygonCentralDisplay.h"
#include "PolygonSegmentSlider.h"
#include <algorithm>
#include <cmath>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <parameters/ParameterTooltip.h>

namespace jnsc::juce_interface {

/**
 * @brief Polygon-based parameter panel.
 *
 * Creates one PolygonSegmentSlider per APVTS ranged parameter and places each
 * slider as an edge in a regular polygon. The center display shows the brand,
 * plugin name, and the last interacted parameter value.
 */
class PolygonPanel : public juce::Component, public juce::TooltipClient {
  public:
    PolygonPanel(juce::AudioProcessorValueTreeState& newApvts,
                 juce::String newPluginName,
                 juce::String newBrandText = "J O N S S O N I C",
                 juce::String newVersionText = {})
        : apvts(newApvts), versionText(std::move(newVersionText)) {
        centralDisplay.setPluginName(newPluginName);
        centralDisplay.setBrandText(newBrandText);
        addAndMakeVisible(centralDisplay);
        buildControls();
    }

    PolygonPanel(juce::AudioProcessorValueTreeState& newApvts,
                 juce::String newPluginName,
                 std::vector<juce::String> newOrderedParamIDs,
                 juce::String newBrandText = "J O N S S O N I C",
                 juce::StringArray newExcludedParamIDs = {},
                 juce::String newVersionText = {})
        : apvts(newApvts), orderedParamIDs(std::move(newOrderedParamIDs)),
          excludedParamIDs(std::move(newExcludedParamIDs)), versionText(std::move(newVersionText)) {
        centralDisplay.setPluginName(newPluginName);
        centralDisplay.setBrandText(newBrandText);
        addAndMakeVisible(centralDisplay);
        buildControls();
    }

    ~PolygonPanel() override = default;

    void paint(juce::Graphics& g) override {
        if (versionText.isEmpty())
            return;

        g.setColour(centralDisplay.getBrandColour().withAlpha(0.6f));
        g.setFont(12.0f);
        g.drawText(versionText, getWidth() - 112, 16, 96, 20, juce::Justification::centredRight);
    }

    void resized() override { layoutControls(); }

    void parentHierarchyChanged() override { updateTooltipWindow(); }

    juce::String getTooltip() override {
        const auto position = getMouseXYRelative();
        for (const auto& segment : segments) {
            if (segment.label != nullptr && segment.label->getBounds().contains(position))
                return segment.slider->getTooltip();
        }
        return {};
    }

    PolygonCentralDisplay& getCentralDisplay() noexcept { return centralDisplay; }

    void setProcessingEnabled(bool enabled) {
        processingEnabled = enabled;
        setEnabled(enabled);
        if (enabled && activeSegment != nullptr) {
            for (const auto& segment : segments) {
                if (segment.slider == activeSegment) {
                    publishActiveValue(*segment.slider, segment.paramName);
                    break;
                }
            }
        }
    }

    void setPowerVisualAmount(float amount) {
        setAlpha(0.35f + 0.65f * amount);
        centralDisplay.setParameterDetailsVisible(amount > 0.0f);
        centralDisplay.setParameterDetailsOpacity(amount);
        for (auto& slider : ownedSliders)
            slider->setPowerGlowAmount(amount);
    }

    PolygonSegmentSlider* getSegmentForParam(const juce::String& paramID) noexcept {
        for (const auto& segment : segments) {
            if (segment.paramID == paramID)
                return segment.slider;
        }
        return nullptr;
    }

  private:
    struct SegmentInfo {
        juce::String paramID;
        juce::String paramName;
        juce::String paramUnit;
        PolygonSegmentSlider* slider = nullptr;
        juce::Label* label = nullptr;
    };

    juce::AudioProcessorValueTreeState& apvts;
    PolygonCentralDisplay centralDisplay;

    std::vector<std::unique_ptr<PolygonSegmentSlider>> ownedSliders;
    std::vector<std::unique_ptr<juce::Label>> ownedLabels;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
    std::vector<SegmentInfo> segments;
    std::vector<juce::String> orderedParamIDs;
    juce::StringArray excludedParamIDs;
    juce::String versionText;
    PolygonSegmentSlider* activeSegment = nullptr;
    bool processingEnabled = true;
    bool hasParameterTooltips = false;
    juce::Component::SafePointer<juce::Component> tooltipParent;
    std::unique_ptr<juce::TooltipWindow> tooltipWindow;

    void updateTooltipWindow() {
        if (!hasParameterTooltips || getParentComponent() == nullptr) {
            tooltipWindow.reset();
            tooltipParent = nullptr;
            return;
        }

        auto* parent = static_cast<juce::Component*>(findParentComponentOfClass<juce::AudioProcessorEditor>());
        if (parent == nullptr)
            parent = getTopLevelComponent();
        if (tooltipWindow != nullptr && tooltipParent.getComponent() == parent)
            return;
        tooltipWindow.reset();
        tooltipParent = parent;
        tooltipWindow = std::make_unique<juce::TooltipWindow>(parent, 2000);
    }

    float polygonRadiusInset = 58.0f;
    // Larger interaction height makes dragging easier without changing track thickness.
    float segmentVisualHeight = 36.0f;
    float labelWidth = 130.0f;
    float labelHeight = 24.0f;
    float labelDistance = 2.0f;
    float sideLabelLetterOffsetPx = 3.0f;
    int sideLabelLetterBaseline = 1;

    void buildControls() {
        hasParameterTooltips = false;
        for (const auto* parameter : apvts.processor.getParameters()) {
            if (getParameterTooltip(*parameter).trim().isNotEmpty()) {
                hasParameterTooltips = true;
                break;
            }
        }
        updateTooltipWindow();
        ownedSliders.clear();
        ownedLabels.clear();
        sliderAttachments.clear();
        segments.clear();

        removeAllChildren();
        addAndMakeVisible(centralDisplay);

        struct ParamInput {
            juce::AudioProcessorParameter* base = nullptr;
            juce::AudioProcessorParameterWithID* withID = nullptr;
            juce::RangedAudioParameter* ranged = nullptr;
        };

        std::vector<ParamInput> allParams;
        allParams.reserve(apvts.processor.getParameters().size());

        for (auto* paramBase : apvts.processor.getParameters()) {
            auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(paramBase);
            auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(paramBase);
            if (paramWithID == nullptr || rangedParam == nullptr)
                continue;
            if (excludedParamIDs.contains(paramWithID->paramID))
                continue;

            allParams.push_back({paramBase, paramWithID, rangedParam});
        }

        std::vector<ParamInput> orderedParams;
        orderedParams.reserve(allParams.size());

        if (orderedParamIDs.empty()) {
            orderedParams = allParams;
        } else {
            juce::StringArray used;
            for (const auto& orderedID : orderedParamIDs) {
                auto it = std::find_if(allParams.begin(), allParams.end(), [&](const auto& p) {
                    return p.withID->paramID == orderedID;
                });
                if (it != allParams.end()) {
                    orderedParams.push_back(*it);
                    used.add(it->withID->paramID);
                }
            }

            for (const auto& p : allParams) {
                if (!used.contains(p.withID->paramID))
                    orderedParams.push_back(p);
            }
        }

        for (const auto& input : orderedParams) {
            auto* paramBase = input.base;
            auto* paramWithID = input.withID;
            auto* rangedParam = input.ranged;

            auto slider = std::make_unique<PolygonSegmentSlider>();
            auto* sliderPtr = slider.get();
            sliderPtr->setName(paramBase->getName(64));
            sliderPtr->setTooltip(getParameterTooltip(*paramBase));

            const auto range = rangedParam->getNormalisableRange();
            const double interval = range.interval > 0.0f ? static_cast<double>(range.interval) : 0.0;
            sliderPtr->setRange(range.start, range.end, interval);
            sliderPtr->setDoubleClickReturnValue(true, rangedParam->convertFrom0to1(rangedParam->getDefaultValue()));
            sliderPtr->setValue(rangedParam->convertFrom0to1(rangedParam->getValue()), juce::dontSendNotification);
            sliderPtr->setTextValueSuffix(" " + rangedParam->getLabel());

            auto label = std::make_unique<juce::Label>();
            auto* labelPtr = label.get();
            labelPtr->setInterceptsMouseClicks(false, false);
            labelPtr->setTooltip(getParameterTooltip(*paramBase));
            labelPtr->setJustificationType(juce::Justification::centred);
            labelPtr->setColour(juce::Label::textColourId, juce::Colour::fromRGB(162, 146, 118).withAlpha(0.56f));
            sliderPtr->onHighlightChange = [labelPtr](float amount) {
                const auto inactive = juce::Colour::fromRGB(162, 146, 118).withAlpha(0.56f);
                const auto active = juce::Colour::fromRGB(252, 232, 199);
                labelPtr->setColour(juce::Label::textColourId, inactive.interpolatedWith(active, amount));
            };
            labelPtr->setText(toSpacedLetters(paramBase->getName(36), true), juce::dontSendNotification);
            auto labelFont = juce::Font(juce::FontOptions("Avenir", 19.0f, juce::Font::plain));
            labelFont.setTypefaceStyle("Light");
            labelPtr->setFont(labelFont);

            addAndMakeVisible(*sliderPtr);
            addAndMakeVisible(*labelPtr);

            const juce::String paramID = paramWithID->paramID;
            const juce::String paramName = paramBase->getName(64);
            const juce::String paramUnit = rangedParam->getLabel();

            sliderPtr->onDragStart = [this, sliderPtr, paramName] { publishActiveValue(*sliderPtr, paramName); };

            sliderPtr->onValueChange = [this, sliderPtr, paramName] { publishActiveValue(*sliderPtr, paramName); };

            sliderPtr->onHover = [this, sliderPtr, paramName] { publishActiveValue(*sliderPtr, paramName); };

            sliderAttachments.emplace_back(
                std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramID, *sliderPtr));

            segments.push_back({paramID, paramName, paramUnit, sliderPtr, labelPtr});
            ownedSliders.push_back(std::move(slider));
            ownedLabels.push_back(std::move(label));
        }

        if (!segments.empty())
            publishActiveValue(*segments.front().slider, segments.front().paramName);
    }

    void layoutControls() {
        const auto bounds = getLocalBounds().toFloat();
        if (bounds.getWidth() < 10.0f || bounds.getHeight() < 10.0f)
            return;

        const auto center = bounds.getCentre();
        const float outerRadius =
            juce::jmax(40.0f, juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f - polygonRadiusInset);

        const int numSegments = static_cast<int>(segments.size());
        if (numSegments == 0) {
            centralDisplay.setBounds(bounds.toNearestInt());
            return;
        }

        std::vector<juce::Point<float>> vertices;
        vertices.reserve(static_cast<size_t>(numSegments));

        const float angleStep = juce::MathConstants<float>::twoPi / static_cast<float>(numSegments);
        // Offset by half a step so the first edge midpoint sits at top center.
        const float startAngle = -juce::MathConstants<float>::halfPi - (angleStep * 0.5f);

        for (int i = 0; i < numSegments; ++i) {
            const float angle = startAngle + angleStep * static_cast<float>(i);
            vertices.emplace_back(center.x + std::cos(angle) * outerRadius, center.y + std::sin(angle) * outerRadius);
        }

        for (int i = 0; i < numSegments; ++i) {
            auto& segment = segments[static_cast<size_t>(i)];
            if (segment.slider == nullptr || segment.label == nullptr)
                continue;

            const auto a = vertices[static_cast<size_t>(i)];
            const auto b = vertices[static_cast<size_t>((i + 1) % numSegments)];
            const auto mid = (a + b) * 0.5f;

            const float dx = b.x - a.x;
            const float dy = b.y - a.y;
            const float edgeLength = juce::jmax(24.0f, std::sqrt(dx * dx + dy * dy));
            const float edgeAngle = std::atan2(dy, dx);

            // Extend the component for the halo while preserving the original track length.
            const int sliderWidth = static_cast<int>(edgeLength) + 2 * PolygonSegmentSlider::glowEndPadding;
            const float sliderCentreX = static_cast<float>(sliderWidth) * 0.5f;
            segment.slider->setBounds(0, 0, sliderWidth, static_cast<int>(segmentVisualHeight));
            segment.slider->setTransform(
                juce::AffineTransform::rotation(edgeAngle, sliderCentreX, segmentVisualHeight * 0.5f)
                    .translated(mid.x - sliderCentreX, mid.y - segmentVisualHeight * 0.5f));

            auto normal = mid - center;
            const auto normalLength = normal.getDistanceFromOrigin();
            if (normalLength > 0.001f)
                normal = normal / normalLength;
            else
                normal = {0.0f, -1.0f};

            // Keep a uniform center-based spacing, then add side-only extra
            // offset for longer labels to avoid side overlap.
            const bool isSideSegment = std::abs(normal.x) > std::abs(normal.y);
            const float supportExtent = labelHeight * 0.5f;
            const int letterCount = countNonWhitespaceChars(segment.label->getText());
            const int extraLetters = juce::jmax(0, letterCount - sideLabelLetterBaseline);
            const float sideLetterOffset =
                isSideSegment ? (static_cast<float>(extraLetters) * sideLabelLetterOffsetPx) : 0.0f;
            const float sliderHalfThickness = segmentVisualHeight * 0.5f;
            const float outwardOffset = sliderHalfThickness + supportExtent + labelDistance + sideLetterOffset;

            const auto labelCenter = mid + normal * outwardOffset;

            segment.label->setBounds(juce::Rectangle<int>(static_cast<int>(labelCenter.x - labelWidth * 0.5f),
                                                          static_cast<int>(labelCenter.y - labelHeight * 0.5f),
                                                          static_cast<int>(labelWidth),
                                                          static_cast<int>(labelHeight)));
        }

        const float centralDiameter = juce::jmax(140.0f, outerRadius * 1.12f);
        centralDisplay.setBounds(static_cast<int>(center.x - centralDiameter * 0.5f),
                                 static_cast<int>(center.y - centralDiameter * 0.5f),
                                 static_cast<int>(centralDiameter),
                                 static_cast<int>(centralDiameter));
        centralDisplay.toBack();
    }

    void publishActiveValue(juce::Slider& slider, const juce::String& paramName) {
        if (!processingEnabled || !isEnabled())
            return;
        const double value = slider.getValue();
        juce::String valueText(static_cast<int>(std::round(value)));

        const auto suffix = slider.getTextValueSuffix().trim();
        if (suffix.isNotEmpty())
            valueText << " " << suffix;

        setActiveSegment(dynamic_cast<PolygonSegmentSlider*>(&slider));
        centralDisplay.setActiveParameter(paramName, valueText);
    }

    void setActiveSegment(PolygonSegmentSlider* slider) {
        if (activeSegment == slider)
            return;
        activeSegment = slider;

        for (auto& segment : segments) {
            const bool isActive = (segment.slider == activeSegment);

            if (segment.slider != nullptr)
                segment.slider->setHighlighted(isActive);
        }
    }

    static juce::String toSpacedLetters(const juce::String& source, bool uppercase) {
        juce::String text = source.trim();
        if (uppercase)
            text = text.toUpperCase();

        juce::String compact;
        for (int i = 0; i < text.length(); ++i) {
            const juce::juce_wchar c = text[i];
            if (!juce::CharacterFunctions::isWhitespace(c))
                compact << juce::String::charToString(c);
        }

        juce::String spaced;
        for (int i = 0; i < compact.length(); ++i) {
            if (i > 0)
                spaced << " ";
            spaced << juce::String::charToString(compact[i]);
        }

        return spaced;
    }

    static int countNonWhitespaceChars(const juce::String& text) {
        int count = 0;
        for (int i = 0; i < text.length(); ++i) {
            if (!juce::CharacterFunctions::isWhitespace(text[i]))
                ++count;
        }
        return count;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolygonPanel)
};

} // namespace jnsc::juce_interface
