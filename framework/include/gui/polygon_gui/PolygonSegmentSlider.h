// Jonssonic Plugin Framework
// Polygon segment slider (one edge of polygon control panel)
// SPDX-License-Identifier: MIT

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace jnsc::juce_interface {

/**
 * @brief Slider that renders as a polygon edge with center-out fill.
 *
 * The visual is intentionally simple:
 * - fixed dark track for all states
 * - center-out capsule fill
 * - diffuse glow around the filled portion
 * - active segment uses a brighter fill colour
 */
class PolygonSegmentSlider : public juce::Slider, private juce::Timer {
  public:
    static constexpr int glowEndPadding = 12;

    std::function<void()> onHover;
    std::function<void(float)> onHighlightChange;

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

        const auto lineBounds = bounds.reduced(hitPadding + static_cast<float>(glowEndPadding), hitPadding);
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

        const float trackTop = centreY - (trackThickness * 0.5f);
        const auto trackRect = juce::Rectangle<float>(startX, trackTop, endX - startX, trackThickness);

        // Always keep empty portions/track at the same dark tone.
        g.setColour(trackColour.withAlpha(trackAlpha));
        g.fillRoundedRectangle(trackRect, trackThickness * 0.5f);

        const auto effectiveFillColour = fillColour.interpolatedWith(activeFillColour, highlightAmount);
        drawGlow(g, fillRect, effectiveFillColour);
        g.setColour(effectiveFillColour);
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
        if (!isEnabled()) {
            stopTimer();
            updateHighlight(isHighlighted ? 1.0f : 0.0f);
        }
        if (!isEnabled() && isDraggingVertically) {
            isDraggingVertically = false;
            if (onDragEnd)
                onDragEnd();
        }
    }

    void setVerticalDragPixels(float newVerticalDragPixels) {
        verticalDragPixels = juce::jmax(1.0f, newVerticalDragPixels);
    }

    void setTrackThickness(float newThickness) {
        trackThickness = juce::jmax(1.0f, newThickness);
        fillThickness = juce::jmax(trackThickness, fillThickness);
        repaint();
    }

    void setFillThickness(float newThickness) {
        fillThickness = juce::jmax(1.0f, newThickness);
        repaint();
    }

    void setTrackColour(juce::Colour newColour) {
        trackColour = newColour;
        repaint();
    }

    void setFillColour(juce::Colour newColour) {
        fillColour = newColour;
        repaint();
    }

    void setActiveFillColour(juce::Colour newColour) {
        activeFillColour = newColour;
        repaint();
    }

    void setThumbColour(juce::Colour newColour) {
        thumbColour = newColour;
        repaint();
    }

    void setGlow(float radius, float opacity) {
        radius = juce::jlimit(0.0f, 8.0f, radius);
        opacity = juce::jlimit(0.0f, 1.0f, opacity);
        if (glowRadius == radius && glowOpacity == opacity)
            return;
        glowRadius = radius;
        glowOpacity = opacity;
        repaint();
    }

    void setGlowEnabled(bool enabled) {
        if (glowEnabled == enabled)
            return;
        glowEnabled = enabled;
        repaint();
    }

    void setPowerGlowAmount(float amount) {
        powerGlowAmount = juce::jlimit(0.0f, 1.0f, amount);
        repaint();
    }

    void setTrackAlpha(float newAlpha) {
        trackAlpha = juce::jlimit(0.0f, 1.0f, newAlpha);
        repaint();
    }

    void setShowThumb(bool shouldShowThumb) {
        showThumb = shouldShowThumb;
        repaint();
    }

    void setHighlighted(bool shouldHighlight) {
        if (isHighlighted == shouldHighlight)
            return;
        isHighlighted = shouldHighlight;
        if (!isEnabled() || !isShowing()) {
            stopTimer();
            updateHighlight(isHighlighted ? 1.0f : 0.0f);
            return;
        }
        highlightStartAmount = highlightAmount;
        highlightStartTime = juce::Time::getMillisecondCounterHiRes();
        startTimerHz(60);
    }

  private:
    void updateHighlight(float amount) {
        highlightAmount = amount;
        if (onHighlightChange)
            onHighlightChange(amount);
        repaint();
    }

    void timerCallback() override {
        const float progress = juce::jlimit(0.0f, 1.0f,
            static_cast<float>((juce::Time::getMillisecondCounterHiRes() - highlightStartTime) / 180.0));
        const float target = isHighlighted ? 1.0f : 0.0f;
        if (progress >= 1.0f || !isEnabled() || !isShowing()) {
            stopTimer();
            updateHighlight(target);
            return;
        }
        const float eased = progress * progress * (3.0f - 2.0f * progress);
        updateHighlight(highlightStartAmount + (target - highlightStartAmount) * eased);
    }

    // Draw complete capsules so transformed sliders have no image-slice seams.
    // A fixed number of translucent layers keeps dragging allocation-free.
    void drawGlow(juce::Graphics& g, juce::Rectangle<float> fill, juce::Colour colour) {
        if (!glowEnabled || powerGlowAmount <= 0.0f || glowRadius <= 0.0f || glowOpacity <= 0.0f)
            return;

        constexpr float weights[] = {0.02f, 0.04f, 0.07f, 0.11f, 0.15f, 0.18f, 0.20f, 0.23f};
        const float radius = glowRadius * (1.0f + 0.5f * highlightAmount);
        const float opacity = powerGlowAmount * juce::jmin(1.0f, glowOpacity * (1.0f + 0.7f * highlightAmount));
        for (int layer = 0; layer < 8; ++layer) {
            const float spread = radius * (static_cast<float>(8 - layer) / 3.0f);
            const auto halo = fill.expanded(spread);
            g.setColour(colour.withMultipliedAlpha(opacity * weights[layer]));
            g.fillRoundedRectangle(halo, juce::jmin(halo.getWidth(), halo.getHeight()) * 0.5f);
        }
    }

    float glowRadius = 3.0f;
    bool glowEnabled = true;
    float powerGlowAmount = 1.0f;
    float glowOpacity = 0.38f;

    float dragStartNorm = 0.0f;
    float dragStartScreenY = 0.0f;
    float verticalDragPixels = 220.0f;
    bool isDraggingVertically = false;

    float trackThickness = 10.0f;
    float fillThickness = 10.0f;
    float trackAlpha = 0.6f;
    float hitPadding = 6.0f;
    bool showThumb = false;
    bool isHighlighted = false;
    float highlightAmount = 0.0f;
    float highlightStartAmount = 0.0f;
    double highlightStartTime = 0.0;

    juce::Colour trackColour = juce::Colour::fromRGB(56, 62, 56);
    juce::Colour fillColour = juce::Colour::fromRGB(210, 160, 108);
    juce::Colour activeFillColour = juce::Colour::fromRGB(234, 188, 132);
    juce::Colour thumbColour = juce::Colour::fromRGB(255, 230, 200);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolygonSegmentSlider)
};

} // namespace jnsc::juce_interface
