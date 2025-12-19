
# Plugin-specific CMake for Compressor plugin

# Add Compressor Plugin target with runtime resource copying
add_plugin(Compressor
    VERSION 0.1.0
    PLUGIN_NAME "Compressor"
    PROD_NAME "Compressor"
    PROD_CODE COMP
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
        knobs/JonssonicRotarySlider_Compressor.png
        
)

# Link Jonssonic DSP and Framework libraries
target_link_libraries(Compressor 
    PRIVATE 
        JonssonicDSP
        JonssonicFramework
)
