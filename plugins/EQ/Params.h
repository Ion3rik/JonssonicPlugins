//==============================================================================
// Jonssonic EQ Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/Parameters.h>
#include <parameters/ParameterGroup.h>
#include <parameters/ParameterTypes.h>

namespace EQParams {

// Parameter IDs as enum
enum class ID {
    // Define your parameter IDs here
    // Example:
    LowCutFreq,
    LowMidGain,
    HighMidGain,
    HighShelfGain,
    LowMidFreq,
    HighMidFreq
};

// Create parameter definitions
inline Jonssonic::ParameterSet<ID> createParams() {
    using namespace Jonssonic;
    
    ParameterSet<ID> params;
    
    // Float parameter        ↓ id                  ↓ name              ↓ min   ↓ max       ↓ def     ↓ unit  ↓ skew
    params.add(FloatParam<ID>{ID::LowCutFreq,      "Low Cut",           20.0f,   1000.0f,   20.0f,    "Hz",    0.5f});
    params.add(FloatParam<ID>{ID::LowMidGain,      "Low Mid Gain",      -15.0f,  15.0f,     0.0f,     "dB",    1.0f});
    params.add(FloatParam<ID>{ID::HighMidGain,     "High Mid Gain",     -15.0f,  15.0f,     0.0f,     "dB",    1.0f});
    params.add(FloatParam<ID>{ID::HighShelfGain,   "High Shelf Gain",   -15.0f,  15.0f,     0.0f,     "dB",    1.0f});
    params.add(FloatParam<ID>{ID::LowMidFreq,      "Low Mid Freq",      50.0f,   2000.0f,   500.0f,   "Hz",    0.7f});  
    params.add(FloatParam<ID>{ID::HighMidFreq,     "High Mid Freq",     500.0f,  8000.0f,   2000.0f,  "Hz",    0.7f});
    return params;
}

} // namespace EQParams
