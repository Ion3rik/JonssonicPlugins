//==============================================================================
// Jonssonic Reverb Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/ParameterGroup.h>
#include <parameters/ParameterSet.h>
#include <parameters/ParameterTypes.h>

struct ReverbParams {

    // Parameter IDs as enum
    enum class ID { PreDelay, Diffusion, ModDepth, ReverbTimeLow, Crossover, ReverbTimeHigh, ModRate, LowCut, Mix };

    // Create parameter definitions
    inline jnsc::juce_interface::ParameterSet<ID> createParams() {
        using namespace jnsc::juce_interface;

        ParameterSet<ID> params;
        // clang-format off
        // Float parameter        ↓ id                 ↓ name              ↓ min   ↓ max       ↓ def     ↓ unit  ↓ skew
        params.add(FloatParam<ID>{ID::ReverbTimeLow,   "RT Low",           0.2f,   10.0f,      2.0f,     "s",    0.5f});
        params.add(FloatParam<ID>{ID::Crossover,       "Crossover",        100.0f, 5000.0f,    1000.0f,  "Hz",   0.5f});
        params.add(FloatParam<ID>{ID::ReverbTimeHigh,  "RT High",          0.2f,   10.0f,      1.0f,     "s",    0.5f});

        params.add(FloatParam<ID>{ID::Diffusion,       "Diffusion",        0.0f,   100.0f,     50.0f,    "%",    1.0f});
        params.add(FloatParam<ID>{ID::ModRate,         "Mod. Rate",        0.1f,   20.0f,       1.0f,    "Hz",    0.5f});
        params.add(FloatParam<ID>{ID::ModDepth,        "Mod. Depth",       0.0f,   100.0f,     10.0f,    "%",    1.0f});

        params.add(FloatParam<ID>{ID::PreDelay,        "Pre-Delay",        0.0f,   200.0f,     0.0f,     "ms",   1.0f});
        params.add(FloatParam<ID>{ID::LowCut,          "Low Cut",          20.0f,  1000.0f,    20.0f,    "Hz",   0.5f});
        params.add(FloatParam<ID>{ID::Mix,             "Mix",              0.0f,   100.0f,     50.0f,    "%",    1.0f});
        // clang-format on
        return params;
    }
};
