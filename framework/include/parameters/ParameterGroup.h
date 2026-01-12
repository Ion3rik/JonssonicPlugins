// Jonssonic Plugin Framework
// Parameter group templates
// Design inspired by mrta_utils from João Rossi's Modern Real-Time Audio (2025)
// https://github.com/joaorossi/modern-real-time-audio-2025/tree/main/modules/mrta_utils
// SPDX-License-Identifier: MIT

#pragma once

#include "Parameters.h"
#include <string>
#include <functional>

namespace jonssonic::juce_framework::parameters {

/**
 * @brief Helper for generating groups of related parameters
 * 
 * Allows defining a parameter structure once and instantiating it
 * multiple times with different IDs and names.
 * 
 * Example:
 *   auto oscGroup = ParameterGroup<ID>()
 *       .addFloat(0, "Frequency", 20.0f, 20000.0f, 440.0f, "Hz", 0.3f)
 *       .addFloat(1, "Level", -60.0f, 0.0f, -12.0f, "dB", 3.0f);
 *   
 *   oscGroup.instantiate(params, {{ID::Osc1_Start, "Osc 1"}, 
 *                                  {ID::Osc2_Start, "Osc 2"}});
 */
template<typename IDType>
class ParameterGroup {
public:
    struct Instance {
        IDType baseID;
        std::string prefix;
    };
    
    ParameterGroup() = default;
    
    /**
     * @brief Add a float parameter template to this group
     * @param offset Offset from base ID (0, 1, 2, etc.)
     * @param name Parameter name (will be prefixed with instance name)
     * @return Reference to this for chaining
     */
    ParameterGroup& addFloat(int offset, std::string name, float min, float max, 
                             float def, std::string unit = "", float skew = 1.0f) {
        generators_.push_back([=](IDType baseID, const std::string& prefix, ParameterSet<IDType>& params) {
            auto id = static_cast<IDType>(static_cast<int>(baseID) + offset);
            params.add(FloatParam<IDType>{id, prefix + " " + name, min, max, def, unit, skew});
        });
        return *this;
    }
    
    /**
     * @brief Add an integer parameter template to this group
     */
    ParameterGroup& addInt(int offset, std::string name, int min, int max, 
                           int def, std::string unit = "") {
        generators_.push_back([=](IDType baseID, const std::string& prefix, ParameterSet<IDType>& params) {
            auto id = static_cast<IDType>(static_cast<int>(baseID) + offset);
            params.add(IntParam<IDType>{id, prefix + " " + name, min, max, def, unit});
        });
        return *this;
    }
    
    /**
     * @brief Add a boolean parameter template to this group
     */
    ParameterGroup& addBool(int offset, std::string name, bool def) {
        generators_.push_back([=](IDType baseID, const std::string& prefix, ParameterSet<IDType>& params) {
            auto id = static_cast<IDType>(static_cast<int>(baseID) + offset);
            params.add(BoolParam<IDType>{id, prefix + " " + name, def});
        });
        return *this;
    }
    
    /**
     * @brief Add a choice parameter template to this group
     */
    ParameterGroup& addChoice(int offset, std::string name, 
                              std::vector<std::string> choices, int def) {
        generators_.push_back([=](IDType baseID, const std::string& prefix, ParameterSet<IDType>& params) {
            auto id = static_cast<IDType>(static_cast<int>(baseID) + offset);
            params.add(ChoiceParam<IDType>{id, prefix + " " + name, choices, def});
        });
        return *this;
    }
    
    /**
     * @brief Instantiate this group for multiple instances
     * @param params Parameter set to add to
     * @param instances List of {baseID, prefix} pairs
     */
    void instantiate(ParameterSet<IDType>& params, 
                     const std::vector<Instance>& instances) const {
        for (const auto& inst : instances) {
            for (const auto& generator : generators_) {
                generator(inst.baseID, inst.prefix, params);
            }
        }
    }
    
    /**
     * @brief Instantiate this group sequentially with automatic numbering
     * @param params Parameter set to add to
     * @param baseID Starting ID
     * @param count Number of instances
     * @param prefix Name prefix (will append " 1", " 2", etc.)
     * @param stride ID stride between instances (default: # params in group)
     */
    void instantiateSequential(ParameterSet<IDType>& params, IDType baseID, 
                               int count, const std::string& prefix, int stride = -1) const {
        if (stride < 0) {
            stride = static_cast<int>(generators_.size());
        }
        
        std::vector<Instance> instances;
        for (int i = 0; i < count; ++i) {
            auto id = static_cast<IDType>(static_cast<int>(baseID) + (i * stride));
            instances.push_back({id, prefix + " " + std::to_string(i + 1)});
        }
        instantiate(params, instances);
    }

private:
    using Generator = std::function<void(IDType, const std::string&, ParameterSet<IDType>&)>;
    std::vector<Generator> generators_;
};

} // namespace Jonssonic
