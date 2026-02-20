# Plugin-specific CMake for SVFDemo plugin
add_plugin(SVFDemo
    VERSION 0.1.0
    PLUGIN_NAME "SVFDemo"
    PROD_NAME "SVFDemo"
    PROD_CODE SVFD
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