# Plugin-specific CMake for Reverb plugin
add_plugin(Reverb
    VERSION 0.1.0
    PLUGIN_NAME "Reverb"
    PROD_NAME "Reverb"
    PROD_CODE REVE
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
        knobs/JonssonicRotarySlider_Reverb.png
)

