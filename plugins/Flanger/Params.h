//==============================================================================
// Jonssonic Flanger Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/ParameterGroup.h>
#include <parameters/ParameterSet.h>
#include <parameters/ParameterTypes.h>

struct FlangerParams {

    // Parameter IDs as enum
    enum class ID { Rate, Depth, Spread, Delay, Feedback, Mix };

    // Create parameter definitions
    inline jnsc::juce_interface::ParameterSet<ID> createParams() {
        using namespace jnsc::juce_interface;
        ParameterSet<ID> params;
        //                        ↓ id              ↓ name          ↓ min   ↓ max   ↓ def   ↓ unit ↓ skew
        // clang-format off
        params.add(FloatParam<ID>{ID::Rate,         "Rate",         0.05f,  2.0f,  0.3f,   "Hz",  0.3f});
        params.add(FloatParam<ID>{ID::Depth,        "Depth",        0.0f,   100.0f, 50.0f,  "%",   1.0f});
        params.add(FloatParam<ID>{ID::Spread,       "Spread",       0.0f,   100.0f, 0.0f,   "%",   1.0f});
        params.add(FloatParam<ID>{ID::Delay,        "Delay",        0.2f,   4.0f,  2.0f,   "ms",  1.0f});
        params.add(FloatParam<ID>{ID::Feedback,     "Feedback",    -100.0f, 100.0f, 25.0f,   "%",   1.0f});
        params.add(FloatParam<ID>{ID::Mix,          "Mix",          0.0f,   100.0f, 100.0f, "%",   1.0f});
        // clang-format on
        return params;
    }
};
