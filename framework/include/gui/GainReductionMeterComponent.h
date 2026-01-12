// Jonssonic Plugin Framework
// Gain reduction meter component
// SPDX-License-Identifier: MIT


#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <memory>
#include <visualizers/GainReductionMeterState.h>

namespace jonssonic::juce_framework::gui {
/**
 * @brief Gain reduction meter GUI component
 * Polls LevelMeterState for level updates and repaints itself.
 */
class GainReductionMeterComponent : public juce::Component, private juce::Timer
{
public:

    /**
     * @brief Construct a GainReductionMeterComponent with a required LevelMeterState.
     * @param state Shared pointer to the state to poll (must not be null)
     * @param refreshHz Polling rate in Hz (default 30)
     */
    explicit GainReductionMeterComponent(std::shared_ptr<const jonssonic::juce_framework::visualizers::GainReductionMeterState> state, int newRefreshHz = 30)
        : gainReductionState(std::move(state)), refreshHz(newRefreshHz)
    {
        setOpaque(false);
        jassert(gainReductionState != nullptr); // Enforce non-null state
        if (gainReductionState)
            startTimerHz(refreshHz);
    }
    ~GainReductionMeterComponent() override = default;


    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();


        constexpr float labelAreaWidth = 20.0f;
        constexpr float verticalPad = 20.0f; // space at top and bottom
        auto paddedBounds = bounds.reduced(0, verticalPad);
        auto meterBarBounds = paddedBounds.withWidth(paddedBounds.getWidth() - labelAreaWidth);
        auto labelArea = paddedBounds.withX(meterBarBounds.getRight()).withWidth(labelAreaWidth);

       
        if (gainReductionState) {
            // DRAW METER BAR
            float minDb = gainReductionState->minDb;
            // Convert linear level to dB
            float db = juce::Decibels::gainToDecibels(level, minDb);
            // Normalize: 0 when db = 0, 1 when db = minDb (max reduction)
            float norm = juce::jlimit(0.0f, 1.0f, juce::jmap(db, 0.0f, minDb, 0.0f, 1.0f));
            float meterHeight = meterBarBounds.getHeight() * norm;
            if (meterHeight > 0.0f) {
                juce::Colour meterColour = juce::Colours::white.withAlpha(0.7f);
                g.setColour(meterColour);
                auto barRect = meterBarBounds.withHeight(meterHeight);
                float cornerRadius = std::min(barRect.getWidth(), barRect.getHeight()) * 0.3f;
                g.fillRoundedRectangle(barRect, cornerRadius);
            }
            // DRAW HOLD INDICATOR
            float holdDb = juce::Decibels::gainToDecibels(holdLevel, gainReductionState->minDb);
            float holdNorm = juce::jlimit(0.0f, 1.0f, juce::jmap(holdDb, gainReductionState->minDb, gainReductionState->maxDb, 0.0f, 1.0f));
            float holdY = meterBarBounds.getY() + meterBarBounds.getHeight() * (1.0f - holdNorm);
            g.setColour(juce::Colours::orange.withAlpha(0.9f));
            g.drawLine(meterBarBounds.getX(), holdY, meterBarBounds.getRight(), holdY, 2.0f);
        }

        // Set up font for tick markers
        juce::Font tickFont(juce::FontOptions("Avenir", 14, juce::Font::bold));
        g.setColour(juce::Colours::white);
        g.setFont(tickFont);

        // Draw dB labels in the reserved label area
        if (gainReductionState) {
            float minDb = gainReductionState->minDb;
            float maxDb = gainReductionState->maxDb;
            int numTicks = 5; // e.g. -60, -45, -30, -15, 0
            float tickLength = 6.0f;
            float labelPad = 0.0f;
            for (int i = 0; i <= numTicks; ++i) {
                float db = juce::jmap((float)i, 0.0f, (float)numTicks, minDb, maxDb);
                float norm = juce::jmap(db, minDb, maxDb, 1.0f, 0.0f); // 1.0 = bottom, 0.0 = top
                float y = paddedBounds.getY() + paddedBounds.getHeight() * norm;
                // Draw dB ticks 
                g.drawText(juce::String((int)db),
                          labelArea.getX() + labelPad, y - 7, labelArea.getWidth() - labelPad, 14, juce::Justification::centredRight);
            }
        }
        
        // Set up font for meter label
        juce::Font labelFont(juce::FontOptions("Avenir", 14, juce::Font::plain));
        g.setColour(juce::Colours::white);
        g.setFont(labelFont);
        // Draw meter label below the meter bar
        if (gainReductionState && !gainReductionState->label.empty()) {
            float labelHeight = 18.0f;
            g.drawText(gainReductionState->label,
                      meterBarBounds.getX(), meterBarBounds.getY() - labelHeight - 2,
                      meterBarBounds.getWidth(), labelHeight,
                      juce::Justification::centred);
        }
    }


    void resized() override {}

    void reset() noexcept {
        setLevel(0.0f);
        holdLevel = 0.0f;
        holdTimer = 0.0;
        repaint();
    }

private:
    // Visual state variables
    int refreshHz = 30;
    float level = 1.0f;
    float holdLevel = 1.0f;
    double holdTimeMs = 1000.0;
    double holdTimer = 0.0;
    // Auto-reset logic
    float lastPolledLevel = 1.0f;
    int silentFrames = 0;
    const int silentFramesThreshold = 5; 

    // Pointer to the shared level meter state
    std::shared_ptr<const jonssonic::juce_framework::visualizers::GainReductionMeterState> gainReductionState;

    /**
     * @brief Set the current level and repaint
     * @param newLevel New level value (0.0 to 1.0)
     */
    void setLevel(float newLevel) {
        level = juce::jlimit(0.0f, 1.0f, newLevel);
        repaint();
    }

    /**
     * @brief Timer callback to poll level state
     * Reads the level from LevelMeterState and updates the display.
     * Also manages the hold level and timer.
     */
    void timerCallback() override {
        if (!gainReductionState) return;
        float newLevel = gainReductionState->level.load(std::memory_order_relaxed);
        DBG("[GainReductionMeterComponent] newLevel: " << newLevel << ", holdLevel: " << holdLevel);
        // Auto-reset: count frames with no change or silence
        if (newLevel == lastPolledLevel) {
            ++silentFrames;
        } else {
            silentFrames = 0;
        }
        lastPolledLevel = newLevel;
        if (silentFrames > silentFramesThreshold) {
            // No new value for a while, auto-reset to 1.0 (0 dB)
            setLevel(1.0f);
            holdLevel = 1.0f;
            holdTimer = 0.0;
            return;
        }
        if (newLevel < holdLevel) {
            holdLevel = newLevel;
            holdTimer = 0.0;
        } else {
            holdTimer += 2000.0 / refreshHz; // increment hold timer
            if (holdTimer > holdTimeMs) {
                holdLevel = std::max(holdLevel - 0.2f, newLevel); // smooth fall
            }
        }
        setLevel(newLevel);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainReductionMeterComponent)
};
} // namespace Jonssonic
