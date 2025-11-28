// Jonssonic Plugin Framework
// Parameter type definitions
// Design inspired by mrta_utils from João Rossi's Modern Real-Time Audio (2025)
// https://github.com/joaorossi/modern-real-time-audio-2025/tree/main/modules/mrta_utils
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <vector>

namespace Jonssonic {

/**
 * @brief Floating-point parameter specification
 * 
 * Defines a continuous parameter with range, default, unit, and skew factor.
 * The skew factor controls the slider's response curve (1.0 = linear).
 */
template<typename IDType>
struct FloatParam {
    IDType id;
    std::string name;
    float min;
    float max;
    float defaultValue;
    std::string unit = "";
    float skew = 1.0f;  // 1.0 = linear, <1.0 = logarithmic, >1.0 = exponential
    
    FloatParam(IDType id, std::string name, float min, float max, float def,
               std::string unit = "", float skew = 1.0f)
        : id(id), name(std::move(name)), min(min), max(max),
          defaultValue(def), unit(std::move(unit)), skew(skew) {}
};

/**
 * @brief Integer parameter specification
 * 
 * Defines a discrete integer parameter with range and default value.
 */
template<typename IDType>
struct IntParam {
    IDType id;
    std::string name;
    int min;
    int max;
    int defaultValue;
    std::string unit = "";
    
    IntParam(IDType id, std::string name, int min, int max, int def, std::string unit = "")
        : id(id), name(std::move(name)), min(min), max(max),
          defaultValue(def), unit(std::move(unit)) {}
};

/**
 * @brief Boolean parameter specification
 * 
 * Defines a simple on/off toggle parameter, with customizable labels for true/false states.
 */
template<typename IDType>
struct BoolParam {
    IDType id;
    std::string name;
    bool defaultValue;
    std::string trueLabel;
    std::string falseLabel;

    BoolParam(IDType id, std::string name, bool def,
              std::string trueLabel = "On", std::string falseLabel = "Off")
        : id(id), name(std::move(name)), defaultValue(def),
          trueLabel(std::move(trueLabel)), falseLabel(std::move(falseLabel)) {}
};

/**
 * @brief Choice/dropdown parameter specification
 * 
 * Defines a parameter with a discrete set of named options.
 */
template<typename IDType>
struct ChoiceParam {
    IDType id;
    std::string name;
    std::vector<std::string> choices;
    int defaultIndex;
    
    ChoiceParam(IDType id, std::string name, std::vector<std::string> choices, int def)
        : id(id), name(std::move(name)), choices(std::move(choices)), defaultIndex(def) {}
};

} // namespace Jonssonic
