//==============================================================================
// Jonssonic Delay Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/ParameterGroup.h>
#include <parameters/ParameterSet.h>
#include <parameters/ParameterTypes.h>

struct DelayParams {

    // Parameter IDs as enum
    enum class ID {
        // Define your parameter IDs here
        // Example:
        DelayTimeMs,
        Feedback,
        PingPong,
        Damping,
        ModDepth,
        Mix
    };

    // Create parameter definitions
    inline jnsc::juce_interface::ParameterSet<ID> createParams() {
        using namespace jnsc::juce_interface;

        ParameterSet<ID> params;
        // clang-format off
    // Float parameter        ↓ id              ↓ name          ↓ min   ↓ max     ↓ def     ↓ unit  ↓ skew
    params.add(FloatParam<ID>{ID::DelayTimeMs,  "Time",         50.0f,   2000.0f,  500.0f,   "ms",   1.0f});
    params.add(FloatParam<ID>{ID::Feedback,     "Feedback",     0.0f,   100.0f,   0.0f,     "%",    1.0f});
    params.add(FloatParam<ID>{ID::PingPong,     "Spread",       0.0f,   100.0f,   0.0f,     "%",    1.0f});
    params.add(FloatParam<ID>{ID::Damping,      "Damping",      0.0f,   100.0f,   0.0f,     "%",    1.0f});
    params.add(FloatParam<ID>{ID::ModDepth,     "Modulation",   0.0f,   100.0f,   0.0f,     "%",    1.0f});
    params.add(FloatParam<ID>{ID::Mix,          "Mix",          0.0f,   100.0f,   50.0f,    "%",    1.0f});
    return params;
        // clang-format on
    }
};
