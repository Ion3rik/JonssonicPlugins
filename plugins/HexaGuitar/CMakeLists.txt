# Plugin-specific CMake for HexaGuitar plugin
add_plugin(HexaGuitar
    VERSION 0.1.0
    PLUGIN_NAME "HexaGuitar"
    PROD_NAME "HexaGuitar"
    PROD_CODE HEXA
    SYNTH FALSE
    ICON_BIG "${CMAKE_SOURCE_DIR}/resources/logos/jonssonic_icon.svg"

    SOURCES
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h
        HexaGuitarFonts.h
        DSP/HexaGuitar.h
        DSP/HexaGuitar.cpp

    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_includes  # if any shared headers exist, otherwise omit
    RESOURCES
        logos/jonssonic_logo.svg
        fonts/Jost/OFL.txt
)

juce_add_binary_data(HexaGuitarFontResources
    NAMESPACE HexaGuitarFontData
    HEADER_NAME HexaGuitarFontData.h
    SOURCES
        ${CMAKE_SOURCE_DIR}/resources/fonts/Jost/JostRegular.ttf
        ${CMAKE_SOURCE_DIR}/resources/fonts/Jost/JostBold.ttf
)
target_link_libraries(HexaGuitar PRIVATE HexaGuitarFontResources)
