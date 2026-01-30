#pragma once
#include <juce_core/juce_core.h>

namespace jnsc::juce_interface {
/**
 * @brief Utility functions for accessing plugin resources.
 */

/**
 * @brief Get resource file path within the plugin bundle
 * @param relativePath Relative path within Resources folder (e.g., "logos/Jonssonic_logo.png")
 * @return juce::File pointing to the resource
 */
inline juce::File getResourceFile(const juce::String& relativePath) {
#if JUCE_MAC
    return juce::File::getSpecialLocation(juce::File::currentApplicationFile)
        .getChildFile("Contents/Resources/")
        .getChildFile(relativePath);
#elif JUCE_WINDOWS
    auto exeDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
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
