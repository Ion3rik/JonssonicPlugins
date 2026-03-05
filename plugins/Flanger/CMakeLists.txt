# Plugin-specific CMake for Flanger
add_plugin(Flanger
    PLUGIN_NAME "Flanger"
    PROD_NAME "Flanger"
    PROD_CODE FLGR
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
        knobs/JonssonicRotarySlider_Flanger.png
)

