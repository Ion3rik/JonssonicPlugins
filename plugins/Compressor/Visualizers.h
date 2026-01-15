// Jonssonic Compressor Plugin - Visualizer Definitions
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <vector>
#include <visualizers/VisualizerSet.h>
#include <visualizers/VisualizerTypes.h>

struct CompressorVisualizers {

    // Enum or struct for visualizer IDs
    enum class ID { GainReduction };

    // Create the list of meters for this plugin
    inline jnsc::juce_interface::VisualizerSet<ID> createVisualizers() {
        using namespace jnsc::juce_interface;
        VisualizerSet<ID> visualizers;

        // clang-format off
        // Add visualizers             			↓ id               ↓ name   ↓ minDb     ↓ maxDb
        visualizers.add(GainReductionMeter<ID>{ ID::GainReduction, "GR", 	-40.0f,		0.0f});
        // clang-format on
        return visualizers;
    }
};
