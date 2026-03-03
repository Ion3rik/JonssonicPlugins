# Plugin-specific CMake for Chorus plugin
add_plugin(Chorus
    VERSION 0.1.0
    PLUGIN_NAME "Chorus"
    PROD_NAME "Chorus"
    PROD_CODE CHOR
    SYNTH FALSE

    SOURCES
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h

    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_includes  # if any shared headers exist, otherwise omit
    RESOURCES
        logos/Jonssonic_logo.png
        knobs/JonssonicRotarySlider.png
)