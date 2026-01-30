// Jonssonic Plugin Framework
// Meter set class
// SPDX-License-Identifier: MIT

#pragma once
#include "VisualizerTypes.h"
#include <stdexcept>
#include <unordered_map>
#include <variant>
#include <vector>

/**
 * @brief Container for all visualizers in the plugin.
 * Holds visualizer definitions and provides type-safe access.
 * Currently supports LevelMeter and GainReductionMeter and can be extended for more visualizer types.
 */
namespace jnsc::juce_interface {

template <typename IDType>
class VisualizerSet {
  public:
    /// Type alias for visualizer variant
    using VisualizerVariant = std::variant<LevelMeter<IDType>, GainReductionMeter<IDType>>;

    /// Default constructor
    VisualizerSet() = default;

    /**
     * @brief Add a level meter to the set.
     * @param levelMeter LevelMeter instance to add.
     */
    void add(LevelMeter<IDType> levelMeter) {
        visualizers.emplace_back(levelMeter);
        indexMap[levelMeter.id] = visualizers.size() - 1;
    }

    /**
     * @brief Add a gain reduction meter to the set.
     * @param gainReductionMeter GainReductionMeter instance to add.
     */
    void add(GainReductionMeter<IDType> gainReductionMeter) {
        visualizers.emplace_back(gainReductionMeter);
        indexMap[gainReductionMeter.id] = visualizers.size() - 1;
    }

    /**
     * @brief Get a visualizer by id.
     * @param id Visualizer ID.
     * @return Visualizer variant corresponding to the given ID.
     * @throws std::runtime_error if the ID is not found.
     */
    const VisualizerVariant& get(IDType id) const {
        auto it = indexMap.find(id);
        if (it == indexMap.end()) {
            throw std::runtime_error("Visualizer ID not found");
        }
        return visualizers[it->second];
    }

    /**
     * @brief Check if a visualizer exists by id.
     * @param id Visualizer ID.
     * @return true if visualizer with given ID exists, false otherwise.
     */
    bool has(IDType id) const { return indexMap.find(id) != indexMap.end(); }

    /**
     * @brief Get all visualizers in the set.
     * @return Vector of all visualizer variants.
     */
    const std::vector<VisualizerVariant>& getAll() const { return visualizers; }

    /**
     * @brief Get number of visualizers
     * @return Number of visualizers in the set.
     */
    size_t size() const { return visualizers.size(); }

  private:
    std::vector<VisualizerVariant> visualizers;
    std::unordered_map<IDType, size_t> indexMap;
};
} // namespace jnsc::juce_interface