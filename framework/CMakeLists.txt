# ============================================================
# Jonssonic Plugin Framework
# ============================================================
# A reusable framework for building JUCE audio plugins with
# parameter management and automatic GUI generation.
# ============================================================

cmake_minimum_required(VERSION 3.21)

project(JonssonicFramework VERSION 0.1.0)

# ============================================================
# Collect source files
# ============================================================

file(GLOB_RECURSE FRAMEWORK_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

file(GLOB_RECURSE FRAMEWORK_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ============================================================
# Framework Library
# ============================================================

add_library(JonssonicFramework STATIC
    ${FRAMEWORK_HEADERS}
    ${FRAMEWORK_SOURCES}
)

add_library(Jonssonic::Framework ALIAS JonssonicFramework)

target_include_directories(JonssonicFramework
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link JUCE dependencies
target_link_libraries(JonssonicFramework
    PUBLIC
        juce::juce_audio_processors
        juce::juce_gui_basics
        juce::juce_data_structures
)

target_compile_features(JonssonicFramework
    PUBLIC
        cxx_std_17
)

# Platform-specific settings
if(APPLE)
    target_link_options(JonssonicFramework PRIVATE "-Wl,-ld_classic")
endif()

message(STATUS "Jonssonic Framework configured as static library")
message(STATUS "  Headers: ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "  Sources: ${CMAKE_CURRENT_SOURCE_DIR}/src")
