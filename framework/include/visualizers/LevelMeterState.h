// Jonssonic Plugin Framework
// Level meter state
// SPDX-License-Identifier: MIT

#pragma once
#include <atomic>
#include <cstddef>
#include <string>

namespace Jonssonic {

/**
 * @brief Thread-safe state for the LevelMeterComponent.
 *
 * Audio thread writes, GUI reads. No locks, no allocations.
 * Extend with more fields as needed (e.g. RMS, hold, timestamp).
 */
struct LevelMeterState {
    /**
     * @brief Atomic variables for level metering
     * @param level Current level (0.0 to 1.0)
     * @param peak Current peak level (0.0 to 1.0)
     * @param hold Hold level (0.0 to 1.0)
     * @param lastUpdateSample Sample index of the last update (for timing purposes)
     */
	std::atomic<float> level { 0.0f }; 
	std::atomic<float> peak  { 0.0f };
	std::atomic<float> hold  { 0.0f };
	std::atomic<size_t> lastUpdateSample { 0 };

    /**
     * @brief dB range for the meter
     */
    float minDb  {-60.0f}; // Minimum dB value for the meter
    float maxDb { 20.0f};  // Maximum dB value for the meter

    /**
     * @brief Label for the meter 
     */

    std::string label;

	/**
     * @brief Clear the meter state
     */
	void clear() noexcept {
		level.store(0.0f, std::memory_order_relaxed);
		peak.store(0.0f, std::memory_order_relaxed);
		hold.store(0.0f, std::memory_order_relaxed);
		lastUpdateSample.store(0, std::memory_order_relaxed);
	}
};

} // namespace Jonssonic