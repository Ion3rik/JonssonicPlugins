//==============================================================================
// Jonssonic Compressor Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/Parameters.h>
#include <parameters/ParameterGroup.h>
#include <parameters/ParameterTypes.h>

namespace CompressorParams {

// Parameter IDs as enum
enum class ID {
    Threshold,
    Attack,
    Release,
    Ratio,
    Mix,
    Output,
    CharacterMode
};

// Create parameter definitions
inline Jonssonic::ParameterSet<ID> createParams() {
    using namespace Jonssonic;
    
    ParameterSet<ID> params;
    
    // Float parameter        ↓ id              ↓ name          ↓ min       ↓ max       ↓ def       ↓ unit  ↓ skew
    params.add(FloatParam<ID>{ID::Threshold,    "Threshold",    -60.0f,     0.0f,       -24.0f,     "dB",   1.0f});
    params.add(FloatParam<ID>{ID::Attack,       "Attack",       0.1f,       200.0f,     10.0f,      "ms",   0.3f});
    params.add(FloatParam<ID>{ID::Release,      "Release",      5.0f,       2000.0f,    200.0f,     "ms",   0.3f});
    params.add(IntParam<ID>{ID::Ratio,          "Ratio",        1,          20,        4,           ": 1"});
    params.add(FloatParam<ID>{ID::Mix,          "Mix",          0.0f,       100.0f,     100.0f,      "%",    1.0f});
    params.add(FloatParam<ID>{ID::Output,       "Output",       -24.0f,     12.0f,      0.0f,       "dB",   1.0f});

    // Bool parameter        ↓ id           ↓ name      ↓ def    ↓ true label   ↓ false label
    params.add(BoolParam<ID>{ID::CharacterMode,    "Character",   false,   "Warm",       "Transparent"});


    return params;
}

} // namespace CompressorParams
