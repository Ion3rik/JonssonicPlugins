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
     * @param minDb Minimum dB value for the meter
     * @param maxDb Maximum dB value for the meter
     */
    float minDb{-60.0f};
    float maxDb{20.0f};

    /// Optional label for the meter
    std::string label;

    /// Clear the meter state (reset level to 1.0)
    void clear() noexcept { level.store(1.0f, std::memory_order_relaxed); }
};

} // namespace jnsc::juce_interface