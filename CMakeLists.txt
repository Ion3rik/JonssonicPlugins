# ============================================================
# CMake Configuration for Jonssonic Audio Plugins
# Author: Jon Fagerström
# SPDX-License-Identifier: MIT
# Description: This CMakeLists.txt sets up the build environment for Jonssonic audio plugins
# using JUCE and the JonssonicDSP library. It supports building universal binaries for macOS,
# Windows, and Linux (limited/unstested).

# ============================================================
# General Project Setup
# ============================================================

cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0175 NEW)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS TRUE)

project(JonssonicPlugins VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) 
set(VST3_OUTPUT_DIR "${CMAKE_BINARY_DIR}/VST3") # Output directory for VST3 plugins

# MacOs specific settings
set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64") # Universal Binary for macOS
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0") # Minimum macOS version
if(APPLE)
    set(xcode_15_linker -Wl,-ld_classic)
else()
    set(xcode_15_linker "")
endif()

# Windows specific settings
if (WIN32)
    set(windows_defines _USE_MATH_DEFINES WIN32_LEAN_AND_MEAN)
endif()

# Linux specific settings
if (LINUX)
    set(linux_defines JUCE_USE_CURL=0 JUCE_JACK=1)
endif()

# Variables for vendor information
set(company_name "Jonssonic")
set(company_copyright "Jonssonic © 2025")
set(company_code "JONS")
set(company_bundle_id "com.jonssonic")


# ============================================================
# JonssonicDSP LIBRARY (Safe Hybrid Mode)
# ============================================================
option(USE_LOCAL_DSP "Use local DSP if present" OFF) # Default to fetching from GitHub
if (USE_LOCAL_DSP AND EXISTS ${CMAKE_SOURCE_DIR}/external/JonssonicDSP/CMakeLists.txt)

    message(STATUS "Using LOCAL DSP from external/JonssonicDSP")
    add_subdirectory(external/JonssonicDSP)

else()

    if (USE_LOCAL_DSP)
        message(WARNING "Local DSP requested but NOT found. Falling back to FetchContent.")
    endif()

    message(STATUS "Fetching DSP from GitHub")

    include(FetchContent)

    FetchContent_Declare(
        mydsp
        GIT_REPOSITORY https://github.com/Ion3rik/JonssonicDSP.git
        GIT_TAG main   # Change to version tag later if needed
    )

    FetchContent_MakeAvailable(mydsp)

endif()

# ============================================================
# JUCE
# ============================================================
include(FetchContent)

FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.2
)

FetchContent_MakeAvailable(juce)

# ============================================================
# Framework Library
# ============================================================
add_subdirectory(framework)

# ============================================================
# Helper Function to Add Plugins
# ============================================================
function(add_plugin target)
    # parse input args
    set(one_value_args TARGET VERSION PLUGIN_NAME PROD_NAME PROD_CODE SYNTH)
    set(multi_value_args SOURCES INCLUDE_DIRS)
    cmake_parse_arguments(AP "" "${one_value_args}" "${multi_value_args}" ${ARGN})

    message(STATUS "Adding JUCE plugin target: ${target}")
    message(STATUS "  PLUGIN_NAME: ${AP_PLUGIN_NAME}")
    message(STATUS "  VERSION: ${AP_VERSION}")
    message(STATUS "  PROD_NAME: ${AP_PROD_NAME}")
    message(STATUS "  PROD_CODE: ${AP_PROD_CODE}")
    message(STATUS "  SYNTH: ${AP_SYNTH}")

    # Add juce plugin target
    juce_add_plugin(${target}
        PRODUCT_NAME ${AP_PROD_NAME}
        VERSION ${AP_VERSION}
        MICROPHONE_PERMISSION_ENABLED TRUE
        COMPANY_COPYRIGHT ${company_copyright}
        COMPANY_NAME ${company_name}
        FORMATS Standalone VST3 AU Standalone
        PLUGIN_NAME ${AP_PLUGIN_NAME}
        PLUGIN_MANUFACTURER_CODE ${company_code}
        PLUGIN_CODE ${AP_PROD_CODE}
        BUNDLE_ID ${company_bundle_id}.${AP_PROD_NAME}
        IS_SYNTH ${AP_SYNTH}
        NEEDS_MIDI_INPUT ${AP_SYNTH}
        NEEDS_MIDI_OUTPUT FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE)

    juce_generate_juce_header(${target})

    target_sources(${target}
        PRIVATE
            ${AP_SOURCES})

    target_include_directories(${target}
        PRIVATE
            ${AP_INCLUDE_DIRS})

    target_compile_features(${target}
        PUBLIC
            cxx_std_17)

    if(WIN32)
        set(ASIO_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/asiosdk")
        if(EXISTS "${ASIO_SDK_PATH}/common")
            message(STATUS "Using local ASIO SDK at ${ASIO_SDK_PATH}")
            target_include_directories(${target} PRIVATE "${ASIO_SDK_PATH}/common")
            target_compile_definitions(${target} PRIVATE JUCE_ASIO=1 _USE_MATH_DEFINES WIN32_LEAN_AND_MEAN)
        else()
            message(STATUS "ASIO SDK not found, building without ASIO")
            target_compile_definitions(${target} PRIVATE JUCE_ASIO=0 _USE_MATH_DEFINES WIN32_LEAN_AND_MEAN)
        endif()
    else()
        target_compile_definitions(${target} PRIVATE JUCE_ASIO=0)
    endif()

    if(APPLE)
        target_link_options(${target} PRIVATE "-Wl,-ld_classic")
    endif()

    target_link_libraries(${target}
        PRIVATE
            juce::juce_audio_utils
            juce::juce_dsp
        PUBLIC
            juce::juce_recommended_config_flags
            juce::juce_recommended_lto_flags)

endfunction(add_plugin)

# ============================================================
# PLUGINS (Only build when this is the top-level project)
# ============================================================
if(PROJECT_IS_TOP_LEVEL)
    message(STATUS "Building example plugins...")
    
    # Scan all folders inside plugins/ and automatically add them as subdirectories
    file(GLOB plugin_dirs RELATIVE ${CMAKE_SOURCE_DIR}/plugins ${CMAKE_SOURCE_DIR}/plugins/*)

    foreach(plugin_dir IN LISTS plugin_dirs)
        if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/${plugin_dir}")
            message(STATUS "Adding plugin directory: ${plugin_dir}")
            add_subdirectory(plugins/${plugin_dir})
        endif()
    endforeach()
else()
    message(STATUS "Skipping example plugins (not top-level project)")
endif()


