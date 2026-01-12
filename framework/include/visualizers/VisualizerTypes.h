
// Jonssonic Plugin Framework
// Visualizer type definitions
// SPDX-License-Identifier: MIT

#pragma once

namespace jonssonic::juce_framework::visualizers {


/**
 * @brief Level meter type specification
 *
 * Defines a level meter visualizer with ID, name, and dB range.
 */
template<typename IDType>
struct LevelMeter {
    IDType id;
    const char* name;
    float minDb = -60.0f;
    float maxDb = 20.0f;

    LevelMeter(IDType id, const char* name, float minDb = -60.0f, float maxDb = 0.0f)
        : id(id), name(name), minDb(minDb), maxDb(maxDb) {}
};

/**
 * @brief Gain reduction meter type specification
 * Defines a gain reduction meter visualizer with ID, name, and dB range.
 */
template<typename IDType>
struct GainReductionMeter {
    IDType id;
    const char* name;
    float minDb = -60.0f;
    float maxDb = 0.0f;

    GainReductionMeter(IDType id, const char* name, float minDb = -60.0f, float maxDb = 0.0f)
        : id(id), name(name), minDb(minDb), maxDb(maxDb) {}
};

} // namespace Jonssonic
