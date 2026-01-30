// Jonssonic Plugin Framework
// Parameter set class
// Design inspired by mrta_utils from João Rossi's Modern Real-Time Audio (2025)
// https://github.com/joaorossi/modern-real-time-audio-2025/tree/main/modules/mrta_utils
// SPDX-License-Identifier: MIT

#pragma once

#include "ParameterTypes.h"
#include <stdexcept>
#include <unordered_map>
#include <variant>
#include <vector>

namespace jnsc::juce_interface {

/**
 * @brief Container for all parameters in a plugin
 *
 * Holds parameter definitions and provides type-safe access.
 * Supports Float, Int, Bool, and Choice parameter types.
 */
template <typename IDType>
class ParameterSet {
  public:
    /// Type alias for parameter variant.
    using ParamVariant = std::variant<FloatParam<IDType>, IntParam<IDType>, BoolParam<IDType>, ChoiceParam<IDType>>;

    /// Default constructor.
    ParameterSet() = default;

    /**
     * @brief Add a float parameter.
     * @param param FloatParam to add.
     */
    void add(FloatParam<IDType> param) {
        params.emplace_back(param);
        indexMap[param.id] = params.size() - 1;
    }

    /**
     * @brief Add an integer parameter.
     * @param param IntParam to add.
     */
    void add(IntParam<IDType> param) {
        params.emplace_back(param);
        indexMap[param.id] = params.size() - 1;
    }

    /**
     * @brief Add a boolean parameter.
     * @param param BoolParam to add.
     */
    void add(BoolParam<IDType> param) {
        params.emplace_back(param);
        indexMap[param.id] = params.size() - 1;
    }

    /**
     * @brief Add a choice parameter.
     * @param param ChoiceParam to add.
     */
    void add(ChoiceParam<IDType> param) {
        params.emplace_back(param);
        indexMap[param.id] = params.size() - 1;
    }

    /**
     * @brief Get parameter by ID.
     * @param id Parameter ID.
     * @return Reference to the parameter variant.
     * @throws std::runtime_error if parameter ID not found.
     */
    const ParamVariant& get(IDType id) const {
        auto it = indexMap.find(id);
        if (it == indexMap.end()) {
            throw std::runtime_error("Parameter ID not found");
        }
        return params[it->second];
    }

    /**
     * @brief Check if parameter exists.
     * @param id Parameter ID.
     * @return true if parameter with given ID exists, false otherwise.
     */
    bool has(IDType id) const { return indexMap.find(id) != indexMap.end(); }

    /**
     * @brief Get all parameters
     * @return Const reference to vector of all parameter variants.
     */
    const std::vector<ParamVariant>& getAll() const { return params; }

    /**
     * @brief Get number of parameters
     * @return Number of parameters in the set.
     */
    size_t size() const { return params.size(); }

  private:
    std::vector<ParamVariant> params;
    std::unordered_map<IDType, size_t> indexMap;
};

} // namespace jnsc::juce_interface
