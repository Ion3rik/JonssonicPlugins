//==============================================================================
// Jonssonic Delay Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/Parameters.h>
#include <parameters/ParameterGroup.h>
#include <parameters/ParameterTypes.h>

namespace DelayParams {

// Parameter IDs as enum
enum class ID {
    // Define your parameter IDs here
    // Example:
    Mix,
    DelayTimeMs,
    Feedback,
    DampingHz,
    PingPong,
    ModDepth
};

// Create parameter definitions
inline Jonssonic::ParameterSet<ID> createParams() {
    using namespace Jonssonic;
    
    ParameterSet<ID> params;
    
    // Float parameter        ↓ id          ↓ name      ↓ min   ↓ max     ↓ def     ↓ unit  ↓ skew
    params.add(FloatParam<ID>{ID::Mix,      "Mix",      0.0f,   100.0f,   50.0f,    "%",    1.0f});
    params.add(FloatParam<ID>{ID::DelayTimeMs, "Time", 1.0f, 2000.0f, 500.0f,   "ms",   1.0f});
    params.add(FloatParam<ID>{ID::Feedback, "Feedback", 0.0f,   100.0f,    0.0f,     "%",    1.0f});
    params.add(FloatParam<ID>{ID::DampingHz, "Damping", 500.0f, 20000.0f, 20000.0f, "Hz",   1.0f});
    params.add(FloatParam<ID>{ID::PingPong, "Ping-Pong", 0.0f, 100.0f, 0.0f,    "%",    1.0f});
    params.add(FloatParam<ID>{ID::ModDepth, "Mod. Depth", 0.0f, 100.0f, 0.0f,    "%",    1.0f});
    return params;
}

} // namespace DelayParams
