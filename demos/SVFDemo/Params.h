//==============================================================================
// Jonssonic SVFDemo Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/ParameterGroup.h>
#include <parameters/ParameterSet.h>
#include <parameters/ParameterTypes.h>

struct SVFDemoParams {

    // Parameter IDs as enum
    enum class ID { Frequency, Q, Response };

    // Create parameter definitions
    inline jnsc::juce_interface::ParameterSet<ID> createParams() {
        using namespace jnsc::juce_interface;

        ParameterSet<ID> params;
        // clang-format off
        // Float parameter        ↓ id              ↓ name           ↓ min   ↓ max     ↓ def     ↓ unit  ↓ skew
        params.add(FloatParam<ID>{ID::Frequency,    "Frequency",     20.0f,  20000.0f,   440.0f,    "Hz",    1.0f});
        params.add(FloatParam<ID>{ID::Q,            "Q",             0.1f,   10.0f,   1.0f,    "",    1.0f});
        // Choice parameter       ↓ id              ↓ name           ↓ choices
        params.add(ChoiceParam<ID>{ID::Response,     "Response",      {"Low Pass", "High Pass", "Band Pass" }, 0});

        // clang-format on
        return params;
    }
};
