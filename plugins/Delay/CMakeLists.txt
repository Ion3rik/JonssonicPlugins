# Plugin-specific CMake for Delay plugin

add_plugin(Delay
    VERSION 0.1.0
    PLUGIN_NAME "Delay"
    PROD_NAME "Delay"
    PROD_CODE DELA
    SYNTH FALSE

    SOURCES
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h

    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_includes  # if any shared headers exist, otherwise omit
)

target_link_libraries(Delay 
    PRIVATE 
        JonssonicDSP
        JonssonicFramework
)
target_include_directories(Delay PRIVATE ${CMAKE_SOURCE_DIR}/external/JonssonicDSP/include)
