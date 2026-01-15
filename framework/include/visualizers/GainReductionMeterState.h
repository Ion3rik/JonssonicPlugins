// Jonssonic Plugin Framework
// Gain reduction meter state
// SPDX-License-Identifier: MIT

#pragma once
#include <atomic>
#include <cstddef>
#include <string>

namespace jnsc::juce_interface {

/**
 * @brief Thread-safe state for the GainReductionMeterComponent.
 *
 * Audio thread writes, GUI reads. No locks, no allocations.
 */
struct GainReductionMeterState {
    /**
     * @brief Atomic variable for gain reduction metering
     * @param level Current gain reduction level (0.0 to 1.0)
     */
    std::atomic<float> level{1.0f};

    /**
     * @brief dB range for the meter
     */
    float minDb{-60.0f}; // Minimum dB value for the meter
    float maxDb{20.0f};  // Maximum dB value for the meter

    /**
     * @brief Label for the meter
     */

    std::string label;

    /**
     * @brief Clear the meter state
     */
    void clear() noexcept { level.store(1.0f, std::memory_order_relaxed); }
};

} // namespace jnsc::juce