//==============================================================================
// Jonssonic BiquadDemo Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/ParameterGroup.h>
#include <parameters/ParameterSet.h>
#include <parameters/ParameterTypes.h>

namespace BiquadDemoParams {

// Parameter IDs as enum
enum class ID {
    // Define your parameter IDs here
    // Example:
    Gain,
    Frequency,
    Q,
    FilterType
};

// Create parameter definitions
inline Jonssonic::ParameterSet<ID> createParams() {
    using namespace Jonssonic;

    ParameterSet<ID> params;

    // Float parameter        ↓ id          ↓ name      ↓ min   ↓ max     ↓ def     ↓ unit  ↓ skew
    params.add(FloatParam<ID>{ID::Gain, "Gain", -20.0f, 20.0f, 0.0f, "dB", 1.0f});
    params.add(FloatParam<ID>{ID::Frequency, "Frequency", 10.0f, 20000.0f, 1000.0f, "Hz", 0.5f});
    params.add(FloatParam<ID>{ID::Q, "Q", 0.1f, 10.0f, 0.707f, "", 1.0f});

    // Choice parameter       ↓ id          ↓ name      ↓ choices                       ↓ def idx
    params.add(
        ChoiceParam<ID>{ID::FilterType,
                        "Filter Type",
                        {"Low Pass", "High Pass", "Band Pass", "All Pass", "Notch", "Peak", "Low Shelf", "High Shelf"},
                        0});

    return params;
}

} // namespace BiquadDemoParams
