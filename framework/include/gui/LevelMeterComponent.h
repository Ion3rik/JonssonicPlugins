// Jonssonic Plugin Framework
// Level meter component
// SPDX-License-Identifier: MIT

#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>
#include <visualizers/LevelMeterState.h>

namespace jnsc::juce_interface {
/**
 * @brief Level meter GUI component
 * Polls LevelMeterState for level updates and repaints itself.
 */
class LevelMeterComponent : public juce::Component, private juce::Timer {
  public:
    /**
     * @brief Construct a LevelMeterComponent with a required LevelMeterState.
     * @param state Shared pointer to the state to poll (must not be null)
     * @param refreshHz Polling rate in Hz (default 30)
     */
    explicit LevelMeterComponent(std::shared_ptr<const LevelMeterState> state, int newRefreshHz = 30)
        : levelState(std::move(state)), refreshHz(newRefreshHz) {
        setOpaque(false);
        jassert(levelState != nullptr); // Enforce non-null state
        if (levelState)
            startTimerHz(refreshHz);
    }

    /// Default destructor
    ~LevelMeterComponent() override = default;

    /**
     * @brief Paint the level meter.
     * @param g Graphics context
     */
    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();

        constexpr float labelAreaWidth = 20.0f;
        constexpr float verticalPad = 20.0f; // space at top and bottom
        auto paddedBounds = bounds.reduced(0, verticalPad);
        auto meterBarBounds = paddedBounds.withWidth(paddedBounds.getWidth() - labelAreaWidth);
        auto labelArea = paddedBounds.withX(meterBarBounds.getRight()).withWidth(labelAreaWidth);

        if (levelState) {
            // DRAW METER BAR
            float minDb = levelState->minDb;
            float maxDb = levelState->maxDb;
            // Convert linear level to dB
            float db = juce::Decibels::gainToDecibels(level, minDb);
            // Normalize dB to 0.0 (minDb) ... 1.0 (maxDb)
            float norm = juce::jlimit(0.0f, 1.0f, juce::jmap(db, minDb, maxDb, 0.0f, 1.0f));
            float meterHeight = meterBarBounds.getHeight() * (1.0f - norm);
            juce::Colour meterColour = juce::Colours::white.withAlpha(0.7f);
            g.setColour(meterColour);
            auto barRect = meterBarBounds.withTop(meterBarBounds.getY() + meterHeight);
            float cornerRadius = std::min(barRect.getWidth(), barRect.getHeight()) * 0.3f;
            g.fillRoundedRectangle(barRect, cornerRadius);

            // DRAW HOLD INDICATOR
            float holdDb = juce::Decibels::gainToDecibels(holdLevel, levelState->minDb);
            float holdNorm =
                juce::jlimit(0.0f, 1.0f, juce::jmap(holdDb, levelState->minDb, levelState->maxDb, 0.0f, 1.0f));
            float holdY = meterBarBounds.getY() + meterBarBounds.getHeight() * (1.0f - holdNorm);
            g.setColour(juce::Colours::orange.withAlpha(0.9f));
            g.drawLine(meterBarBounds.getX(), holdY, meterBarBounds.getRight(), holdY, 2.0f);
        }

        // Set up font for tick markers
        juce::Font tickFont(juce::FontOptions("Avenir", 14, juce::Font::bold));
        g.setColour(juce::Colours::white);
        g.setFont(tickFont);

        // Draw dB labels in the reserved label area
        if (levelState) {
            float minDb = levelState->minDb;
            float maxDb = levelState->maxDb;
            int numTicks = 5; // e.g. -60, -45, -30, -15, 0
            float tickLength = 6.0f;
            float labelPad = 0.0f;
            for (int i = 0; i <= numTicks; ++i) {
                float db = juce::jmap((float)i, 0.0f, (float)numTicks, minDb, maxDb);
                float norm = juce::jmap(db, minDb, maxDb, 1.0f, 0.0f); // 1.0 = bottom, 0.0 = top
                float y = paddedBounds.getY() + paddedBounds.getHeight() * norm;
                // Draw dB ticks
                g.drawText(juce::String((int)db),
                           labelArea.getX() + labelPad,
                           y - 7,
                           labelArea.getWidth() - labelPad,
                           14,
                           juce::Justification::centredRight);
            }
        }

        // Set up font for meter label
        juce::Font labelFont(juce::FontOptions("Avenir", 14, juce::Font::plain));
        g.setColour(juce::Colours::white);
        g.setFont(labelFont);
        // Draw meter label below the meter bar
        if (levelState && !levelState->label.empty()) {
            float labelHeight = 18.0f;
            g.drawText(levelState->label,
                       meterBarBounds.getX(),
                       meterBarBounds.getY() - labelHeight - 2,
                       meterBarBounds.getWidth(),
                       labelHeight,
                       juce::Justification::centred);
        }
    }

    /// Resize does nothing special
    void resized() override {}

  private:
    // Visual state variables
    int refreshHz = 30;
    float level = 0.0f;
    float holdLevel = 0.0f;
    double holdTimeMs = 1000.0;
    double holdTimer = 0.0;

    // Pointer to the shared level meter state
    std::shared_ptr<const LevelMeterState> levelState;

    // Set the current level (0.0 to 1.0)
    void setLevel(float newLevel) {
        level = juce::jlimit(0.0f, 1.0f, newLevel);
        repaint();
    }

    // Timer callback to poll the LevelMeterState
    void timerCallback() override {
        if (!levelState)
            return;
        float newLevel = levelState->level.load(std::memory_order_relaxed);
        if (newLevel > holdLevel) {
            holdLevel = newLevel;
            holdTimer = 0.0;
        } else {
            holdTimer += 1000.0 / refreshHz; // increment hold timer
            if (holdTimer > holdTimeMs) {
                holdLevel = std::max(holdLevel - 0.02f, newLevel); // smooth fall
            }
        }
        setLevel(newLevel);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterComponent)
};
} // namespace jnsc::juce_interface
