//==============================================================================
// Jonssonic HexaAmp Plugin Parameters
//==============================================================================

#pragma once

#include <parameters/ParameterGroup.h>
#include <parameters/ParameterSet.h>
#include <parameters/ParameterTypes.h>

struct HexaAmpParams {

    // Parameter IDs as enum
    enum class ID {
        Drive,
        Bite,
        Squish,
        Level,
        Color,
        Tilt,
        Bypass,
        PreAmpCompressorAttack = 8,
        PreAmpCompressorRelease,
        PowerAmpCompressorAttack,
        PowerAmpCompressorRelease,
        MidBoostFrequency,
        PowerAmpShape = 14,
        PreAmpCompressorThresholdMin,
        PreAmpCompressorThresholdMax,
        PowerAmpCompressorThresholdMin,
        PowerAmpCompressorThresholdMax,
        Makeup = 21
    };

    // Create parameter definitions
    inline jnsc::juce_interface::ParameterSet<ID> createParams() {
        using namespace jnsc::juce_interface;

        ParameterSet<ID> params;
        // clang-format off
        // Float parameter        ↓ id          ↓ name      ↓ min   ↓ max     ↓ def     ↓ unit  ↓ skew
        params.add(FloatParam<ID>{ID::Drive,      "Drive",      0.0f,   100.0f,   50.0f,    "%",    1.0f});
        params.add(FloatParam<ID>{ID::Bite,       "Bite",       0.0f,   100.0f,   50.0f,    "%",    1.0f});
        params.add(FloatParam<ID>{ID::Squish,     "Squish",       0.0f,   100.0f,    0.0f,    "%",    1.0f});
        params.add(FloatParam<ID>{ID::Level,      "Level",      0.0f,   100.0f,   50.0f,    "%",    1.0f});
        params.add(FloatParam<ID>{ID::Color,      "Color",      0.0f,   100.0f,   50.0f,    "%",    1.0f});
        params.add(FloatParam<ID>{ID::Tilt,       "Tilt",       0.0f,   100.0f,   50.0f,    "%",    1.0f});
        params.add(BoolParam<ID>{ID::Bypass, "Bypass", false});
        params.add(FloatParam<ID>{ID::PreAmpCompressorAttack, "Pre Amp Compressor Attack", 0.1f, 100.0f, 5.0f, "ms", 0.4f, false});
        params.add(FloatParam<ID>{ID::PreAmpCompressorRelease, "Pre Amp Compressor Release", 5.0f, 1500.0f, 120.0f, "ms", 0.4f, false});
        params.add(FloatParam<ID>{ID::PowerAmpCompressorAttack, "Power Amp Compressor Attack", 0.1f, 200.0f, 30.0f, "ms", 0.4f, false});
        params.add(FloatParam<ID>{ID::PowerAmpCompressorRelease, "Power Amp Compressor Release", 5.0f, 2000.0f, 250.0f, "ms", 0.4f, false});
        params.add(FloatParam<ID>{ID::MidBoostFrequency, "Bite Frequency", 600.0f, 1500.0f, 900.0f, "Hz", 0.5f, false});
        params.add(FloatParam<ID>{ID::PowerAmpShape, "Power Amp Shape", 1.5f, 5.0f, 2.0f, "", 1.0f, false});
        params.add(FloatParam<ID>{ID::PreAmpCompressorThresholdMin, "Pre Amp Compressor TH Min", -60.0f, 0.0f, -32.0f, "dB", 1.0f, false});
        params.add(FloatParam<ID>{ID::PreAmpCompressorThresholdMax, "Pre Amp Compressor TH Max", -60.0f, 0.0f, -8.0f, "dB", 1.0f, false});
        params.add(FloatParam<ID>{ID::PowerAmpCompressorThresholdMin, "Power Amp Compressor TH Min", -60.0f, 0.0f, -24.0f, "dB", 1.0f, false});
        params.add(FloatParam<ID>{ID::PowerAmpCompressorThresholdMax, "Power Amp Compressor TH Max", -60.0f, 0.0f, 0.0f, "dB", 1.0f, false});
        params.add(FloatParam<ID>{ID::Makeup, "Makeup", 0.0f, 48.0f, 18.0f, "dB", 1.0f, false});
        // Optional descriptions shared by the main GUI and internal trims.
        params.setTooltip(ID::Drive, "Sets the drive gain into the power stage.");
        params.setTooltip(ID::Bite, "Boosts or cuts the mids before drive.");
        params.setTooltip(ID::Squish, "Controls amount of compression.");
        params.setTooltip(ID::Level, "Sets output volume.");
        params.setTooltip(ID::Color, "Morphs a parametric cabinet filter from full/dark at 0% to lean/bright at 100%.");
        params.setTooltip(ID::Tilt, "Tilts the preamp tone: lower values are darker, higher values are brighter.");
        return params;
    }
};
