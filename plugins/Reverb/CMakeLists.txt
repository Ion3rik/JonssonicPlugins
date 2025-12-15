# Plugin-specific CMake for Reverb plugin

add_plugin(Reverb
    VERSION 0.1.0
    PLUGIN_NAME "Reverb"
    PROD_NAME "Reverb"
    PROD_CODE REVE
    SYNTH FALSE

    SOURCES
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h

    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_includes  # if any shared headers exist, otherwise omit
)

target_link_libraries(Reverb 
    PRIVATE 
        JonssonicDSP
        JonssonicFramework
)
target_include_directories(Reverb PRIVATE ${CMAKE_SOURCE_DIR}/external/JonssonicDSP/include)
