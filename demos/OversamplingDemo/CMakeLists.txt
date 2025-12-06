# Plugin-specific CMake for OversamplingDemo plugin

add_plugin(OversamplingDemo
    VERSION 0.1.0
    PLUGIN_NAME "OversamplingDemo"
    PROD_NAME "OversamplingDemo"
    PROD_CODE TEMP
    SYNTH FALSE

    SOURCES
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h

    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_includes  # if any shared headers exist, otherwise omit
)

target_link_libraries(OversamplingDemo 
    PRIVATE 
        JonssonicDSP
        JonssonicFramework
)
target_include_directories(OversamplingDemo PRIVATE ${CMAKE_SOURCE_DIR}/external/JonssonicDSP/include)
