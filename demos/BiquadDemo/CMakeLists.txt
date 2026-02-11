# Plugin-specific CMake for BiquadDemo plugin

add_plugin(BiquadDemo
    VERSION 0.1.0
    PLUGIN_NAME "BiquadDemo"
    PROD_NAME "BiquadDemo"
    PROD_CODE BqDm
    SYNTH FALSE

    SOURCES
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h

    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_includes  # if any shared headers exist, otherwise omit
)
