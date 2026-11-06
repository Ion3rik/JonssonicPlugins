# Plugin-specific CMake for Phaser
add_plugin(Phaser
    VERSION 0.3.0
    PLUGIN_NAME "Phaser"
    PROD_NAME "Phaser"
    PROD_CODE PHSR
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
        knobs/JonssonicRotarySlider_Phaser.png
)

