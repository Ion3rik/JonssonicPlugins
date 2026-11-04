// Jonssonic Plugin Framework
// Polygon segment slider (one edge of polygon control panel)
// SPDX-License-Identifier: MIT

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace jnsc::juce_interface {

/**
 * @brief Slider that renders as a polygon edge with center-out fill.
 *
 * Renders a solid center-out capsule fill.
 */
class PolygonSegmentSlider : public juce::Slider {
  public:
    static constexpr int endPadding = 12;

    std::function<void()> onHover;

    PolygonSegmentSlider() : juce::Slider(juce::Slider::LinearHorizontal, juce::Slider::NoTextBox) {
        setRange(0.0, 1.0, 0.0);
        setDoubleClickReturnValue(true, 0.5);
        setMouseDragSensitivity(250);
        setVelocityBasedMode(false);
        setSliderSnapsToMousePosition(false);
        setWantsKeyboardFocus(false);
    }

    ~PolygonSegmentSlider() override = default;

    void paint(juce::Graphics& g) override {
        const auto bounds = getLocalBounds().toFloat();
        if (bounds.getWidth() <= 0.0f || bounds.getHeight() <= 0.0f)
            return;

        const auto lineBounds = bounds.reduced(hitPadding + static_cast<float>(endPadding), hitPadding);
        if (lineBounds.getWidth() <= 0.0f || lineBounds.getHeight() <= 0.0f)
            return;

        const float centreY = lineBounds.getCentreY();
        const float startX = lineBounds.getX();
        const float endX = lineBounds.getRight();
        const float centreX = lineBounds.getCentreX();

        const float valueNorm = juce::jlimit(0.0f, 1.0f, static_cast<float>(valueToProportionOfLength(getValue())));
        const float minimumWidth = juce::jmin(fillThickness, endX - startX);
        const float activeWidth = minimumWidth + (endX - startX - minimumWidth) * valueNorm;

        const float fillTop = centreY - (fillThickness * 0.5f);
        const auto fillRect =
            juce::Rectangle<float>(centreX - (activeWidth * 0.5f), fillTop, activeWidth, fillThickness);

        const auto track = juce::Rectangle<float>(startX, fillTop, endX - startX, fillThickness).expanded(3.0f);
        g.setColour(fillColour);
        g.drawRoundedRectangle(track, track.getHeight() * 0.5f, 1.0f);
        g.fillRoundedRectangle(fillRect, fillThickness * 0.5f);
    }

    void resized() override {}

    void mouseEnter(const juce::MouseEvent& event) override {
        if (!isEnabled())
            return;
        juce::Slider::mouseEnter(event);
        if (onHover)
            onHover();
    }

    void mouseDown(const juce::MouseEvent& event) override {
        if (!isEnabled())
            return;
        dragStartNorm = valueToProportionOfLength(getValue());
        dragStartScreenY = static_cast<float>(event.getScreenPosition().y);
        isDraggingVertically = true;

        if (onDragStart)
            onDragStart();
    }

    void mouseDrag(const juce::MouseEvent& event) override {
        if (!isEnabled() || !isDraggingVertically)
            return;

        const float currentScreenY = static_cast<float>(event.getScreenPosition().y);
        const float deltaY = dragStartScreenY - currentScreenY;
        const float deltaNorm = deltaY / verticalDragPixels;
        const float newNorm = juce::jlimit(0.0f, 1.0f, dragStartNorm + deltaNorm);
        setValue(proportionOfLengthToValue(newNorm), juce::sendNotificationSync);
    }

    void mouseUp(const juce::MouseEvent&) override {
        if (!isDraggingVertically)
            return;
        isDraggingVertically = false;
        if (onDragEnd)
            onDragEnd();
    }

    void enablementChanged() override {
        juce::Slider::enablementChanged();
        if (!isEnabled() && isDraggingVertically) {
            isDraggingVertically = false;
            if (onDragEnd)
                onDragEnd();
        }
    }

    void setVerticalDragPixels(float newVerticalDragPixels) {
        verticalDragPixels = juce::jmax(1.0f, newVerticalDragPixels);
    }

    void setFillThickness(float newThickness) {
        fillThickness = juce::jmax(1.0f, newThickness);
        repaint();
    }

    void setFillColour(juce::Colour newColour) {
        fillColour = newColour;
        repaint();
    }

  private:
    float dragStartNorm = 0.0f;
    float dragStartScreenY = 0.0f;
    float verticalDragPixels = 220.0f;
    bool isDraggingVertically = false;

    float fillThickness = 10.0f;
    float hitPadding = 6.0f;

    juce::Colour fillColour = juce::Colours::white;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolygonSegmentSlider)
};

} // namespace jnsc::juce_interface
