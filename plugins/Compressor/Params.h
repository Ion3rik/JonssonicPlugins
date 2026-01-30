//==============================================================================
// Jonssonic Compressor Plugin Parameters
//==============================================================================
#pragma once

#include <parameters/ParameterGroup.h>
#include <parameters/ParameterSet.h>
#include <parameters/ParameterTypes.h>

struct CompressorParams {

    // Parameter IDs as enum
    enum class ID {
        Threshold,
        Ratio,
        Knee,
        Attack,
        Release,
        Output,
    };

    // Create parameter definitions
    inline jnsc::juce_interface::ParameterSet<ID> createParams() {
        using namespace jnsc::juce_interface;
        ParameterSet<ID> params;

        // clang-format off
    // Float parameter        ↓ id              ↓ name          ↓ min       ↓ max       ↓ def       ↓ unit      ↓ skew
    params.add(FloatParam<ID>{ID::Threshold,    "Threshold",    -60.0f,     0.0f,       -18.0f,     "dB",       1.0f});
    params.add(IntParam<ID>{  ID::Ratio,        "Ratio",        1,          20,         4,          ": 1"});
    params.add(FloatParam<ID>{ID::Knee,         "Knee",         0.0f,       12.0f,      0.0f,       "dB",       1.0f});
    params.add(FloatParam<ID>{ID::Attack,       "Attack",       0.1f,       50.0f,      10.0f,      "ms",       0.25f});
    params.add(FloatParam<ID>{ID::Release,      "Release",      10.0f,      1000.0f,    50.0f,      "ms",       0.25f});
    params.add(FloatParam<ID>{ID::Output,       "Output",       -12.0f,     24.0f,      0.0f,       "dB",       1.0f});
        // clang-format on

        return params;
    }
};
