//==============================================================================
// Jonssonic HexaAmp Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/ParameterGroup.h>
#include <parameters/ParameterSet.h>
#include <parameters/ParameterTypes.h>

struct HexaAmpParams {

    // Parameter IDs as enum
    enum class ID { Drive, Bite, Squish, Level, Color, Tilt, Bypass };

    // Create parameter definitions
    inline jnsc::juce_interface::ParameterSet<ID> createParams() {
        using namespace jnsc::juce_interface;

        ParameterSet<ID> params;
        // clang-format off
        // Float parameter        ↓ id          ↓ name      ↓ min   ↓ max     ↓ def     ↓ unit  ↓ skew
        params.add(FloatParam<ID>{ID::Drive,      "Drive",      0.0f,   100.0f,   50.0f,    "%",    1.0f});
        params.add(FloatParam<ID>{ID::Bite,       "Bite",       0.0f,   100.0f,   50.0f,    "%",    1.0f});
        params.add(FloatParam<ID>{ID::Squish,     "Squish",     0.0f,   100.0f,   50.0f,    "%",    1.0f});
        params.add(FloatParam<ID>{ID::Level,      "Level",      0.0f,   100.0f,   50.0f,    "%",    1.0f});
        params.add(FloatParam<ID>{ID::Color,      "Color",      0.0f,   100.0f,   50.0f,    "%",    1.0f});
        params.add(FloatParam<ID>{ID::Tilt,       "Tilt",       0.0f,   100.0f,   50.0f,    "%",    1.0f});
        params.add(BoolParam<ID>{ID::Bypass, "Bypass", false});
        return params;
    }
};
