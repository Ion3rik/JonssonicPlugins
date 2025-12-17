//==============================================================================
// Jonssonic Reverb Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/Parameters.h>
#include <parameters/ParameterGroup.h>
#include <parameters/ParameterTypes.h>

namespace ReverbParams {

// Parameter IDs as enum
enum class ID {
    PreDelay,
    ReverbTimeLow,
    ReverbTimeHigh,
    Diffusion,
    LowCut,
    Mix
};

// Create parameter definitions
inline Jonssonic::ParameterSet<ID> createParams() {
    using namespace Jonssonic;
    
    ParameterSet<ID> params;

    // Float parameter        ↓ id                 ↓ name              ↓ min   ↓ max     ↓ def     ↓ unit  ↓ skew
    params.add(FloatParam<ID>{ID::PreDelay,        "Pre-Delay",        0.0f,   200.0f,     0.0f,     "ms",   1.0f});
    params.add(FloatParam<ID>{ID::ReverbTimeLow,   "RT Low",           0.2f,   10.0f,      2.0f,     "s",    1.0f});
    params.add(FloatParam<ID>{ID::ReverbTimeHigh,  "RT High",          0.2f,   10.0f,      1.0f,     "s",    1.0f});
    params.add(FloatParam<ID>{ID::Diffusion,       "Diffusion",        0.0f,   100.0f,     50.0f,    "%",    1.0f});
    params.add(FloatParam<ID>{ID::LowCut,          "Low Cut",          20.0f,  1000.0f,    20.0f,    "Hz",   0.5f});
    params.add(FloatParam<ID>{ID::Mix,             "Mix",              0.0f,   100.0f,     50.0f,    "%",    1.0f});

    return params;
}

} // namespace ReverbParams
