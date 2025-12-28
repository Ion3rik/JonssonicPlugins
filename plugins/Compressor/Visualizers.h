// Jonssonic Compressor Plugin - Visualizer Definitions
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <vector>
#include <visualizers/VisualizerTypes.h>
#include <visualizers/VisualizerSet.h>

namespace CompressorVisualizers {

// Enum or struct for visualizer IDs
enum class ID {
	GainReduction
};


// Create the list of meters for this plugin
inline Jonssonic::VisualizerSet<ID> createVisualizers() {
    using namespace Jonssonic;
	VisualizerSet<ID> visualizers;

    // Add visualizers             ↓ id               ↓ name     ↓ minDb      ↓ maxDb
	visualizers.add(GainReductionMeter<ID>{ID::GainReduction, "GR",      -40.0f,      0.0f});
    
	return visualizers;
}

} // namespace CompressorVisualizers
