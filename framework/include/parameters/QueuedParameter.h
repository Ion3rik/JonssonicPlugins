// Jonssonic Plugin Framework
// Queued Parameter - Safe deferred parameter changes
// SPDX-License-Identifier: MIT

#pragma once

#include <atomic>
#include <optional>

namespace jnsc::juce_interface {

/**
 * @brief Thread-safe parameter queue for deferred changes
 *
 * Use this for parameters that require runtime allocation or heavy
 * reinitialization (e.g., oversampling factor, buffer sizes).
 *
 * The audio thread checks for pending changes at a safe point
 * (e.g., start of processBlock) and applies them without blocking.
 *
 * Supports several deferral strategies:
 * - Immediate (next block)
 * - After N blocks
 * - During silence
 * - Custom condition
 *
 * Example usage:
 * @code
 *   QueuedParameter<int> oversamplingFactor;
 *
 *   // GUI thread: queue a change
 *   paramManager.on(ParamID::Oversampling, [this](int value, bool skip) {
 *       oversamplingFactor.set(value);
 *   });
 *
 *   // Audio thread: apply immediately
 *   if (auto newFactor = oversamplingFactor.getAndClear()) {
 *       oversampler.setOversamplingFactor(*newFactor);
 *   }
 *
 *   // Or wait for silence:
 *   if (auto newFactor = oversamplingFactor.getIfSilent(buffer, -60.0f)) {
 *       oversampler.setOversamplingFactor(*newFactor);
 *   }
 *
 *   // Or wait N blocks:
 *   if (auto newFactor = oversamplingFactor.getAfterBlocks(10)) {
 *       oversampler.setOversamplingFactor(*newFactor);
 *   }
 * @endcode
 */
template <typename T>
class QueuedParameter {
  public:
    QueuedParameter() = default;

    /**
     * @brief Queue a new value (thread-safe, call from GUI or callback)
     * @param value New value to queue
     */
    void set(T value) {
        pendingValue.store(value);
        hasChange.store(true, std::memory_order_release);
    }

    /**
     * @brief Check if there's a pending change
     * @return True if a change is queued
     */
    bool hasPendingChange() const { return hasChange.load(std::memory_order_acquire); }

    /**
     * @brief Get pending value if available (does not clear)
     * @return Optional containing the pending value, or std::nullopt if none
     */
    std::optional<T> get() const {
        if (hasChange.load(std::memory_order_acquire)) {
            return pendingValue.load();
        }
        return std::nullopt;
    }

    /**
     * @brief Get and clear pending value (thread-safe, call from audio thread)
     * @return Optional containing the pending value, or std::nullopt if none
     */
    std::optional<T> getAndClear() {
        if (hasChange.load(std::memory_order_acquire)) {
            T value = pendingValue.load();
            hasChange.store(false, std::memory_order_release);
            return value;
        }
        return std::nullopt;
    }

    /**
     * @brief Clear pending change without retrieving value
     */
    void clear() {
        hasChange.store(false, std::memory_order_release);
        blocksRemaining.store(0);
    }

    /**
     * @brief Get pending value after waiting N blocks (auto-countdown)
     * @param numBlocks Number of blocks to wait before applying
     * @return Optional containing the pending value if ready, or std::nullopt
     *
     * Call this every block. It will count down and return the value when ready.
     * Resets counter if a new value is queued.
     */
    std::optional<T> getAfterBlocks(int numBlocks) {
        if (!hasChange.load(std::memory_order_acquire))
            return std::nullopt;

        // Initialize countdown on first call after new value
        int current = blocksRemaining.load();
        if (current == 0) {
            blocksRemaining.store(numBlocks);
            return std::nullopt;
        }

        // Countdown
        int remaining = blocksRemaining.fetch_sub(1) - 1;
        if (remaining <= 0) {
            T value = pendingValue.load();
            hasChange.store(false, std::memory_order_release);
            blocksRemaining.store(0);
            return value;
        }

        return std::nullopt;
    }

    /**
     * @brief Get pending value if audio buffer is silent (below threshold)
     * @param buffer Audio buffer to check for silence
     * @param thresholdDb Silence threshold in dB (e.g., -60.0f)
     * @return Optional containing the pending value if silent, or std::nullopt
     */
    template <typename BufferType>
    std::optional<T> getIfSilent(const BufferType& buffer, float thresholdDb) {
        if (!hasChange.load(std::memory_order_acquire))
            return std::nullopt;

        float maxLevel = buffer.getMagnitude(0, buffer.getNumSamples());
        float thresholdLinear = std::pow(10.0f, thresholdDb / 20.0f);

        if (maxLevel < thresholdLinear) {
            T value = pendingValue.load();
            hasChange.store(false, std::memory_order_release);
            blocksRemaining.store(0);
            return value;
        }

        return std::nullopt;
    }

    /**
     * @brief Get pending value if custom condition is met
     * @param condition Function returning true when safe to apply
     * @return Optional containing the pending value if condition met, or std::nullopt
     */
    template <typename ConditionFunc>
    std::optional<T> getIf(ConditionFunc condition) {
        if (!hasChange.load(std::memory_order_acquire))
            return std::nullopt;

        if (condition()) {
            T value = pendingValue.load();
            hasChange.store(false, std::memory_order_release);
            blocksRemaining.store(0);
            return value;
        }

        return std::nullopt;
    }

  private:
    std::atomic<T> pendingValue{};
    std::atomic<bool> hasChange{false};
    std::atomic<int> blocksRemaining{0};
};

} // namespace jnsc::juce_interface
