# Plugin-specific CMake for EQ plugin

add_plugin(EQ
    VERSION 0.1.0
    PLUGIN_NAME "EQ"
    PROD_NAME "EQ"
    PROD_CODE EQXX
    SYNTH FALSE

    SOURCES
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h

    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_includes  # if any shared headers exist, otherwise omit
)

target_link_libraries(EQ 
    PRIVATE 
        JonssonicDSP
        JonssonicFramework
)
target_include_directories(EQ PRIVATE ${CMAKE_SOURCE_DIR}/external/JonssonicDSP/include)
