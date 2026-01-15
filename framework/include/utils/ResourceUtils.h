#pragma once
#include <juce_core/juce_core.h>

namespace jnsc::juce_interface {
// Returns the full path to a resource file (e.g., "logos/Jonssonic_logo.png")
// Handles macOS bundles, Windows VST3, and Standalone
inline juce::File getResourceFile(const juce::String& relativePath) {
#if JUCE_MAC
    return juce::File::getSpecialLocation(juce::File::currentApplicationFile)
        .getChildFile("Contents/Resources/")
        .getChildFile(relativePath);
#elif JUCE_WINDOWS
    auto exeDir =
        juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    juce::File vst3Bundle = exeDir.getParentDirectory().getParentDirectory();
    if (vst3Bundle.getFileName().endsWithIgnoreCase(".vst3")) {
        return vst3Bundle.getChildFile("Resources").getChildFile(relativePath);
    } else {
        return exeDir.getChildFile("Resources").getChildFile(relativePath);
    }
#else
    // Fallback: try next to the executable
    return juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory()
        .getChildFile("Resources/")
        .getChildFile(relativePath);
#endif
}
} // namespace jnsc::juce_interface
