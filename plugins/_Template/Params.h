//==============================================================================
// Jonssonic Template Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/ParameterGroup.h>
#include <parameters/ParameterTypes.h>
#include <parameters/Parameters.h>

struct TemplateParams {

    // Parameter IDs as enum
    enum class ID { Mix, Enable, Mode };

    // Create parameter definitions
    inline jnsc::juce_interface::ParameterSet<ID> createParams() {
        using namespace jnsc::juce_interface;

        ParameterSet<ID> params;
        // clang-format off
        // Float parameter        ↓ id          ↓ name      ↓ min   ↓ max     ↓ def     ↓ unit  ↓ skew
        params.add(FloatParam<ID>{ID::Mix,      "Mix",      0.0f,   100.0f,   50.0f,    "%",    1.0f});

        // Bool parameter        ↓ id           ↓ name      ↓ def    ↓ true label   ↓ false label
        params.add(BoolParam<ID>{ID::Enable,    "Enable",   false,    "On",         "Off"});

        // Choice parameter       ↓ id          ↓ name      ↓ choices                       ↓ def idx
        params.add(ChoiceParam<ID>{ID::Mode,    "Mode",     {"Mode1", "Mode2", "Mode3"},    0});
        // clang-format on
        return params;
    }
};
