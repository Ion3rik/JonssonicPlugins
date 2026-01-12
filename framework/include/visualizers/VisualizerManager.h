// Jonssonic Plugin Framework
// VisualizerManager: Centralized management for all visualizers and value suppliers
// SPDX-License-Identifier: MIT

#pragma once
#include "VisualizerSet.h"
#include <unordered_map>
#include <memory>
#include <visualizers/LevelMeterState.h>
#include <visualizers/GainReductionMeterState.h>

namespace jonssonic::juce_framework::visualizers {
/**
 * @brief Centralized visualizer manager
 * 
 * Features:
 * - Holds and manages multiple visualizers
 * - Provides access to visualizer values by ID
 * - Facilitates integration between DSP and GUI components
 * Usage:
 *   VisualizerManager<VisualizerID> visualizerManager(createVisualizers());
 *   visualizerManager.registerValueSupplier(VisualizerID::OutputLevel, [&]() { return dsp.getOutputLevel(); });
 */

template<typename IDType>
class VisualizerManager {
public:
    using ValueSupplier = std::function<float()>;

    VisualizerManager(const VisualizerSet<IDType>& visualizers)
        : visualizerSet(visualizers)
    {
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

    ~VisualizerManager() = default;

    // Register a value supplier for a visualizer
    void registerValueSupplier(IDType id, ValueSupplier supplier) {
        suppliers[id] = std::move(supplier);
    }


    /**
     * @brief Get shared pointer to a visualizer state by ID
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
     * This should be called from the audio thread regularly (e.g., once per processBlock)
     */
    void update() noexcept {
        for (auto& [id, supplier] : suppliers) {
            float v = supplier ? supplier() : 0.0f;
            if (const auto* stateVariant = getState(id)) {
                std::visit([v](auto& ptr) {
                    if (ptr) ptr->level.store(v, std::memory_order_relaxed);
                }, *stateVariant);
            }
        }
    }

    const VisualizerSet<IDType>& getVisualizerSet() const {
        return visualizerSet;
    }

    void clearStates() noexcept {
        for (auto& [id, stateVariant] : states) {
            std::visit([](auto& ptr) {
                if (ptr) ptr->clear();
            }, stateVariant);
        }
    }

private:
    VisualizerSet<IDType> visualizerSet;
    std::unordered_map<IDType, StateVariant> states;
    std::unordered_map<IDType, ValueSupplier> suppliers;
};

} // namespace Jonssonic§