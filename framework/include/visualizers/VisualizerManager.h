// Jonssonic Plugin Framework
// VisualizerManager: Centralized management for all visualizers and value suppliers
// SPDX-License-Identifier: MIT

#pragma once
#include "VisualizerSet.h"
#include <memory>
#include <unordered_map>
#include <visualizers/GainReductionMeterState.h>
#include <visualizers/LevelMeterState.h>

namespace jnsc::juce_interface {
/**
 * @brief Centralized visualizer manager
 *
 * Features:
 * - Holds and manages multiple visualizers
 * - Provides access to visualizer values by ID
 * - Facilitates integration between DSP and GUI components
 * Usage:
 *   VisualizerManager<VisualizerID> visualizerManager(createVisualizers());
 *   visualizerManager.registerValueSupplier(VisualizerID::OutputLevel, [&]() { return
 * dsp.getOutputLevel(); });
 */

template <typename IDType>
class VisualizerManager {
  public:
    /// Type alias for value supplier function
    using ValueSupplier = std::function<float()>;

    /**
     * @brief Construct VisualizerManager with a given VisualizerSet.
     * @param visualizers VisualizerSet defining the visualizers to manage.
     */
    VisualizerManager(const VisualizerSet<IDType>& visualizers) : visualizerSet(visualizers) {
        // Helper for overloaded lambdas
        struct {
            VisualizerManager* self;
            void operator()(const LevelMeter<IDType>& visualizer) const {
                auto state = std::make_shared<LevelMeterState>();
                state->minDb = visualizer.minDb;
                state->maxDb = visualizer.maxDb;
                state->label = visualizer.name;
                self->states[visualizer.id] = state;
            }
            void operator()(const GainReductionMeter<IDType>& visualizer) const {
                auto state = std::make_shared<GainReductionMeterState>();
                state->minDb = visualizer.minDb;
                state->maxDb = visualizer.maxDb;
                state->label = visualizer.name;
                self->states[visualizer.id] = state;
            }
        } visitor{this};
        for (const auto& variant : visualizerSet.getAll()) {
            std::visit(visitor, variant);
        }
    }

    /// Destructor
    ~VisualizerManager() = default;

    /**
     * @brief Register a value supplier for a visualizer by ID.
     * @param id Visualizer ID
     * @param supplier Function that supplies the current value for the visualizer.
     */
    void registerValueSupplier(IDType id, ValueSupplier supplier) { suppliers[id] = std::move(supplier); }

    /**
     * @brief Get shared pointer to a visualizer state by ID
     * @param id Visualizer ID
     * @return Pointer to the visualizer state variant, or nullptr if not found
     */
    using StateVariant = std::variant<std::shared_ptr<LevelMeterState>, std::shared_ptr<GainReductionMeterState>>;
    const StateVariant* getState(IDType id) const {
        auto it = states.find(id);
        if (it != states.end())
            return &it->second;
        return nullptr;
    }

    /**
     * @brief Update all visualizer states by polling their value suppliers
     * @note This should be called from the audio thread regularly (e.g., once per processBlock)
     */
    void update() noexcept {
        for (auto& [id, supplier] : suppliers) {
            float v = supplier ? supplier() : 0.0f;
            if (const auto* stateVariant = getState(id)) {
                std::visit(
                    [v](auto& ptr) {
                        if (ptr)
                            ptr->level.store(v, std::memory_order_relaxed);
                    },
                    *stateVariant);
            }
        }
    }

    /// Get the VisualizerSet
    const VisualizerSet<IDType>& getVisualizerSet() const { return visualizerSet; }

    /// Clear all visualizer states
    void clearStates() noexcept {
        for (auto& [id, stateVariant] : states) {
            std::visit(
                [](auto& ptr) {
                    if (ptr)
                        ptr->clear();
                },
                stateVariant);
        }
    }

  private:
    VisualizerSet<IDType> visualizerSet;
    std::unordered_map<IDType, StateVariant> states;
    std::unordered_map<IDType, ValueSupplier> suppliers;
};

} // namespace jnsc::juce_interface