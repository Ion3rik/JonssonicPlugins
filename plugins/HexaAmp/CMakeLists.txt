# Plugin-specific CMake for HexaAmp plugin
add_plugin(HexaAmp
    VERSION 0.1.0
    PLUGIN_NAME "HexaAmp"
    PROD_NAME "HexaAmp"
    PROD_CODE HEXA
    SYNTH FALSE

    SOURCES
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h
        HexaAmpFonts.h
        DSP/HexaAmp.h
        DSP/HexaAmp.cpp

    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_includes  # if any shared headers exist, otherwise omit
    RESOURCES
        logos/Jonssonic_logo.png
        fonts/Jost/OFL.txt
)

juce_add_binary_data(HexaAmpFontResources
    NAMESPACE HexaAmpFontData
    HEADER_NAME HexaAmpFontData.h
    SOURCES
        ${CMAKE_SOURCE_DIR}/resources/fonts/Jost/JostRegular.ttf
        ${CMAKE_SOURCE_DIR}/resources/fonts/Jost/JostBold.ttf
)
target_link_libraries(HexaAmp PRIVATE HexaAmpFontResources)
