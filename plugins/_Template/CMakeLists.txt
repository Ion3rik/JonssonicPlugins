# Plugin-specific CMake for Template plugin
add_plugin(Template
    VERSION 0.1.0
    PLUGIN_NAME "Template"
    PROD_NAME "Template"
    PROD_CODE TEMP
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