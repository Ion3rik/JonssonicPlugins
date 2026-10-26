# Plugin-specific CMake for HexaAmp plugin
add_plugin(HexaAmp
    PLUGIN_NAME "HexaAmp"
    PROD_NAME "HexaAmp"
    PROD_CODE HEXA
    SYNTH FALSE

    SOURCES
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h
        DSP/HexaAmp.h
        DSP/HexaAmp.cpp

    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_includes  # if any shared headers exist, otherwise omit
    RESOURCES
        logos/Jonssonic_logo.png
)
