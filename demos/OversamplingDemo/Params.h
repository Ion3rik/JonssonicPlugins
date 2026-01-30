//==============================================================================
// Jonssonic OversamplingDemo Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/ParameterGroup.h>
#include <parameters/ParameterSet.h>
#include <parameters/ParameterTypes.h>

namespace OversamplingDemoParams {

// Parameter IDs as enum
enum class ID {
    // Define your parameter IDs here
    // Example:
    Drive,
    OutputGain,
    OversamplingFactor,
};

// Create parameter definitions
inline Jonssonic::ParameterSet<ID> createParams() {
    using namespace Jonssonic;

    ParameterSet<ID> params;

    // Float parameter        ↓ id          ↓ name      ↓ min   ↓ max     ↓ def     ↓ unit  ↓ skew
    params.add(FloatParam<ID>{ID::Drive, "Drive", 0.0f, 24.0f, 0.0f, "dB", 1.0f});
    params.add(FloatParam<ID>{ID::OutputGain, "Output Gain", -24.0f, 24.0f, 0.0f, "dB", 1.0f});

    // Choice parameter       ↓ id          ↓ name      ↓ choices                       ↓ def idx
    params.add(ChoiceParam<ID>{ID::OversamplingFactor, "Oversampling Factor", {"1x", "2x", "4x", "8x", "16x"}, 0});

    return params;
}

} // namespace OversamplingDemoParams
