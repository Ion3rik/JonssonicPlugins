# Plugin-specific CMake for Flanger

add_plugin(Flanger
    VERSION 0.1.0
    PLUGIN_NAME "Jonssonic Flanger"
    PROD_NAME "Jonssonic Flanger"
    PROD_CODE FLGR
    SYNTH FALSE

    SOURCES
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h

    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_includes  # if any shared headers exist, otherwise omit
)
