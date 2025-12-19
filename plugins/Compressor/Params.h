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
    // Define your parameter IDs here
    // Example:
    Mix,
    Enable,
    Mode
};

// Create parameter definitions
inline Jonssonic::ParameterSet<ID> createParams() {
    using namespace Jonssonic;
    
    ParameterSet<ID> params;
    
    // Float parameter        ↓ id          ↓ name      ↓ min   ↓ max     ↓ def     ↓ unit  ↓ skew
    params.add(FloatParam<ID>{ID::Mix,      "Mix",      0.0f,   100.0f,   50.0f,    "%",    1.0f});

    // Bool parameter        ↓ id           ↓ name      ↓ def    ↓ true label   ↓ false label
    params.add(BoolParam<ID>{ID::Enable,    "Enable",   false,    "On",         "Off"});

    // Choice parameter       ↓ id          ↓ name      ↓ choices                       ↓ def idx
    params.add(ChoiceParam<ID>{ID::Mode,    "Mode",     {"Mode1", "Mode2", "Mode3"},    0});

    return params;
}

} // namespace CompressorParams
