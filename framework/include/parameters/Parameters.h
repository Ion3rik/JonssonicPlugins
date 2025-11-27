// Jonssonic Plugin Framework
// Parameter container
// Design inspired by mrta_utils from João Rossi's Modern Real-Time Audio (2025)
// https://github.com/joaorossi/modern-real-time-audio-2025/tree/main/modules/mrta_utils
// SPDX-License-Identifier: MIT

#pragma once

#include "ParameterTypes.h"
#include <variant>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace Jonssonic {

/**
 * @brief Container for all parameters in a plugin
 * 
 * Holds parameter definitions and provides type-safe access.
 * Supports Float, Int, Bool, and Choice parameter types.
 */
template<typename IDType>
class ParameterSet {
public:
    using ParamVariant = std::variant<
        FloatParam<IDType>,
        IntParam<IDType>,
        BoolParam<IDType>,
        ChoiceParam<IDType>
    >;
    
    ParameterSet() = default;
    
    /**
     * @brief Add a float parameter
     */
    void add(FloatParam<IDType> param) {
        params_.emplace_back(param);
        indexMap_[param.id] = params_.size() - 1;
    }
    
    /**
     * @brief Add an integer parameter
     */
    void add(IntParam<IDType> param) {
        params_.emplace_back(param);
        indexMap_[param.id] = params_.size() - 1;
    }
    
    /**
     * @brief Add a boolean parameter
     */
    void add(BoolParam<IDType> param) {
        params_.emplace_back(param);
        indexMap_[param.id] = params_.size() - 1;
    }
    
    /**
     * @brief Add a choice parameter
     */
    void add(ChoiceParam<IDType> param) {
        params_.emplace_back(param);
        indexMap_[param.id] = params_.size() - 1;
    }
    
    /**
     * @brief Get parameter by ID
     */
    const ParamVariant& get(IDType id) const {
        auto it = indexMap_.find(id);
        if (it == indexMap_.end()) {
            throw std::runtime_error("Parameter ID not found");
        }
        return params_[it->second];
    }
    
    /**
     * @brief Check if parameter exists
     */
    bool has(IDType id) const {
        return indexMap_.find(id) != indexMap_.end();
    }
    
    /**
     * @brief Get all parameters
     */
    const std::vector<ParamVariant>& getAll() const {
        return params_;
    }
    
    /**
     * @brief Get number of parameters
     */
    size_t size() const {
        return params_.size();
    }

private:
    std::vector<ParamVariant> params_;
    std::unordered_map<IDType, size_t> indexMap_;
};

} // namespace Jonssonic
