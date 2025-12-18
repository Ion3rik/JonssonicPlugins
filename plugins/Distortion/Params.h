//==============================================================================
// Jonssonic Distortion Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/Parameters.h>
#include <parameters/ParameterGroup.h>
#include <parameters/ParameterTypes.h>

namespace DistortionParams {

// Parameter IDs as enum
enum class ID {
    // Define your parameter IDs here
    // Example:
    Drive,
    Asymmetry,
    Shape,
    Tone,
    Mix,
    Output,
    Oversampling
};

// Create parameter definitions
inline Jonssonic::ParameterSet<ID> createParams() {
    using namespace Jonssonic;
    
    ParameterSet<ID> params;
    
    // Float parameter        ↓ id          ↓ name      ↓ min    ↓ max      ↓ def       ↓ unit  ↓ skew
    params.add(FloatParam<ID>{ID::Drive,   "Drive",     0.0f,    48.0f,     6.0f,       "dB",   1.0f});
    params.add(FloatParam<ID>{ID::Asymmetry,"Asymmetry",      -100.0f, 100.0f,    0.0f,       "",     1.0f});
    params.add(FloatParam<ID>{ID::Shape,   "Shape",     0.0f,    100.0f,    30.0f,      "%",    0.5f});
    params.add(FloatParam<ID>{ID::Tone,    "Tone",      1000.0f, 20000.0f,  12000.0f,   "Hz",   0.5f});
    params.add(FloatParam<ID>{ID::Mix,     "Mix",       0.0f,    100.0f,    100.0f,     "%",    1.0f});
    params.add(FloatParam<ID>{ID::Output,  "Output",    -24.0f,  12.0f,     0.0f,       "dB",   1.0f});

    // Boolean parameter
    params.add(BoolParam<ID>{ID::Oversampling, "Oversampling", false});

    return params;
}

} // namespace DistortionParams
