// Jonssonic Plugin Framework
// Meter set class
// SPDX-License-Identifier: MIT

#pragma once
#include "VisualizerTypes.h"
#include <variant>
#include <vector>
#include <unordered_map>
#include <stdexcept>

/**
 * @brief Container for all visualizers in the plugin.
 *
 * Holds visualizer definitions and provides type-safe access.
 * Currently supports LevelMeter and can be extended for more visualizer types.
 */
namespace Jonssonic {

template<typename IDType>
class VisualizerSet {
public:
	using VisualizerVariant = std::variant<
		LevelMeter<IDType>,
		GainReductionMeter<IDType>
		// Add more visualizer types here as needed
	>;

	VisualizerSet() = default;

	/**
	 * @brief Add a level meter to the set
	 */
	void add(LevelMeter<IDType> levelMeter) {
		visualizers.emplace_back(levelMeter);
		indexMap[levelMeter.id] = visualizers.size() - 1;
	}

	/**
	 * @brief Add a gain reduction meter to the set
	 */
	void add(GainReductionMeter<IDType> gainReductionMeter) {
		visualizers.emplace_back(gainReductionMeter);
		indexMap[gainReductionMeter.id] = visualizers.size() - 1;
	}

	/**
	 * @brief Get a visualizer by id
	 */
	const VisualizerVariant& get(IDType id) const {
		auto it = indexMap.find(id);
		if (it == indexMap.end()) {
			throw std::runtime_error("Visualizer ID not found");
		}
		return visualizers[it->second];
	}

	/**
	 * @brief Check if a visualizer exists by id
	 */
	bool has(IDType id) const {
		return indexMap.find(id) != indexMap.end();
	}

	/**
	 * @brief Get all visualizers
	 */
	const std::vector<VisualizerVariant>& getAll() const {
		return visualizers;
	}

	/**
	 * @brief Get number of visualizers
	 */
	size_t size() const {
		return visualizers.size();
	}

private:
	std::vector<VisualizerVariant> visualizers;
	std::unordered_map<IDType, size_t> indexMap;
};
} // namespace Jonssonic