# Plugin-specific CMake for Distortion plugin
add_plugin(Distortion
    VERSION 0.3.0
    PLUGIN_NAME "Distortion"
    PROD_NAME "Distortion"
    PROD_CODE DIST
    SYNTH FALSE

    SOURCES
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h

    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_includes  # if any shared headers exist, otherwise omit
    RESOURCES
        logos/jonssonic_logo.svg
        knobs/JonssonicRotarySlider.png
        knobs/JonssonicRotarySlider_Distortion.png
)