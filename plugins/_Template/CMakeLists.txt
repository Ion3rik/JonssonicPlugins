# Plugin-specific CMake for Template plugin

add_plugin(Template
    VERSION 0.1.0
    PLUGIN_NAME "Jonssonic Template"
    PROD_NAME "Jonssonic Template"
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

target_link_libraries(Template 
    PRIVATE 
        JonssonicDSP
        JonssonicFramework
)
target_include_directories(Template PRIVATE ${CMAKE_SOURCE_DIR}/external/JonssonicDSP/include)
