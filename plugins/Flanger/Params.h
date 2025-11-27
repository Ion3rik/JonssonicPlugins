//==============================================================================
// Jonssonic Flanger Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/Parameters.h>
#include <parameters/ParameterGroup.h>
#include <parameters/ParameterTypes.h>

namespace FlangerParams {

// Parameter IDs as enum
enum class ID {
    Rate,
    Depth,
    Spread,
    CenterDelay,
    Feedback,
    Mix
};

// Create parameter definitions
inline Jonssonic::ParameterSet<ID> createParams() {
    using namespace Jonssonic;
    
    ParameterSet<ID> params;
    
    //                        ↓ id            ↓ name          ↓ min   ↓ max   ↓ def  ↓ unit ↓ skew
    params.add(FloatParam<ID>{ID::Rate,         "Rate",         0.1f,   10.0f,  1.0f,  "Hz",  0.3f});
    params.add(FloatParam<ID>{ID::Depth,        "Depth",        0.0f,   1.0f,   0.5f,  "",    1.0f});
    params.add(FloatParam<ID>{ID::Spread,       "Spread",       0.0f,   1.0f,   0.5f,  "",    1.0f});
    params.add(FloatParam<ID>{ID::CenterDelay,  "Center Delay", 0.5f,   5.0f,   2.0f,  "ms",  1.0f});
    params.add(FloatParam<ID>{ID::Feedback,     "Feedback",     -0.95f, 0.95f,  0.0f,  "",    1.0f});
    params.add(FloatParam<ID>{ID::Mix,          "Mix",          0.0f,   1.0f,   0.5f,  "%",   1.0f});
    
    return params;
}

} // namespace Flanger
