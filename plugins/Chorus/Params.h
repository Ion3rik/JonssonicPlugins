//==============================================================================
// Jonssonic Chorus Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/ParameterGroup.h>
#include <parameters/ParameterSet.h>
#include <parameters/ParameterTypes.h>

struct ChorusParams {

    // Parameter IDs as enum
    enum class ID { Feedback, Rate, Depth, Delay, Spread, Mix };

    // Create parameter definitions
    inline jnsc::juce_interface::ParameterSet<ID> createParams() {
        using namespace jnsc::juce_interface;

        ParameterSet<ID> params;
        // clang-format off
        // Float parameter        ↓ id           ↓ name      ↓ min   ↓ max    ↓ def    ↓ unit  ↓ skew
        params.add(FloatParam<ID>{ID::Rate,      "Rate",     0.1f,   5.0f,    1.0f,    "Hz",   1.0f});
        params.add(FloatParam<ID>{ID::Depth,     "Depth",    0.0f,   100.0f,  50.0f,   "%",    1.0f});  
        params.add(FloatParam<ID>{ID::Spread,    "Spread",   0.0f,   100.0f,  0.0f,    "%",    1.0f});
        params.add(FloatParam<ID>{ID::Delay,     "Delay",    10.0f,  30.0f,   20.0f,   "ms",   1.0f});
        params.add(FloatParam<ID>{ID::Feedback,  "Feedback", 0.0f,   100.0f,  0.0f,    "%",    1.0f});
        params.add(FloatParam<ID>{ID::Mix,       "Mix",      0.0f,   100.0f,  50.0f,   "%",    1.0f});
        // clang-format on
        return params;
    }
};
