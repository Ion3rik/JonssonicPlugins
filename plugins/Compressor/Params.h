//==============================================================================
// Jonssonic Compressor Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/Parameters.h>
#include <parameters/ParameterGroup.h>
#include <parameters/ParameterTypes.h>

namespace jonssonic::plugins::compressor::params {

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
inline jonssonic::juce_framework::parameters::ParameterSet<ID> createParams() {
    using namespace jonssonic::juce_framework::parameters;
    
    ParameterSet<ID> params;
    
    // Float parameter        ↓ id              ↓ name          ↓ min       ↓ max       ↓ def       ↓ unit      ↓ skew
    params.add(FloatParam<ID>{ID::Threshold,    "Threshold",    -60.0f,     0.0f,       -18.0f,     "dB",       1.0f});
    params.add(IntParam<ID>{  ID::Ratio,        "Ratio",        1,          20,         4,          ": 1"});
    params.add(FloatParam<ID>{ID::Knee,         "Knee",         0.0f,       12.0f,      0.0f,       "dB",       1.0f});
    params.add(FloatParam<ID>{ID::Attack,       "Attack",       0.1f,       50.0f,      10.0f,      "ms",       0.25f});
    params.add(FloatParam<ID>{ID::Release,      "Release",      10.0f,      1000.0f,    50.0f,      "ms",       0.25f});
    params.add(FloatParam<ID>{ID::Output,       "Output",       -12.0f,     24.0f,      0.0f,       "dB",       1.0f});


    return params;
}

} // namespace jonssonic::plugins::compressor
