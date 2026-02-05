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


# Project version setup
project(JonssonicPlugins VERSION 0.1.0)
configure_file(
    ${CMAKE_SOURCE_DIR}/framework/include/Version.h.in
    ${CMAKE_BINARY_DIR}/generated/Version.h
    @ONLY
)

include_directories(${CMAKE_BINARY_DIR}/generated)

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
    
    set(JONSSONIC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(JONSSONIC_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

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
    set(multi_value_args SOURCES INCLUDE_DIRS RESOURCES)
    cmake_parse_arguments(AP "" "${one_value_args}" "${multi_value_args}" ${ARGN})

    message(STATUS "Adding JUCE plugin target: ${target}")
    message(STATUS "  PLUGIN_NAME: ${AP_PLUGIN_NAME}")
    message(STATUS "  VERSION: ${AP_VERSION}")
    message(STATUS "  PROD_NAME: ${AP_PROD_NAME}")
    message(STATUS "  PROD_CODE: ${AP_PROD_CODE}")
    message(STATUS "  SYNTH: ${AP_SYNTH}")
    if(AP_RESOURCES)
        message(STATUS "  RESOURCES: ${AP_RESOURCES}")
    endif()

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
        set(ASIO_SDK_PATH "${CMAKE_SOURCE_DIR}/external/asiosdk")
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
            juce::juce_audio_devices
            juce::juce_core
            juce::juce_audio_processors
            juce::juce_gui_basics
            juce::juce_graphics
        PUBLIC
            juce::juce_recommended_config_flags
            juce::juce_recommended_lto_flags)

    # ===================== Resource Copy Logic =====================
    if(AP_RESOURCES)
        set(_formats Standalone AU VST3)
        foreach(_fmt IN LISTS _formats)
            set(_real_target "${target}_${_fmt}")
            if(TARGET ${_real_target})
                # Copy individual resources for Apple (bundle structure)
                if(APPLE)
                    foreach(_res IN LISTS AP_RESOURCES)
                        get_filename_component(_res_dir "${_res}" DIRECTORY)
                        if(_res_dir)
                            add_custom_command(TARGET ${_real_target} POST_BUILD
                                COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_BUNDLE_DIR:${_real_target}>/Contents/Resources/${_res_dir}"
                                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/resources/${_res}" "$<TARGET_BUNDLE_DIR:${_real_target}>/Contents/Resources/${_res_dir}"
                                COMMENT "Copying resource ${_res} to bundle for ${_real_target}"
                                VERBATIM)
                        else()
                            add_custom_command(TARGET ${_real_target} POST_BUILD
                                COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_BUNDLE_DIR:${_real_target}>/Contents/Resources"
                                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/resources/${_res}" "$<TARGET_BUNDLE_DIR:${_real_target}>/Contents/Resources"
                                COMMENT "Copying resource ${_res} to bundle for ${_real_target}"
                                VERBATIM)
                        endif()
                    endforeach()
                else()
                    # For Windows and other platforms, copy only the specified resource files to the installed VST3 location
                    foreach(_res IN LISTS AP_RESOURCES)
                        get_filename_component(_res_dir "${_res}" DIRECTORY)
                        if(_res_dir)
                            add_custom_command(TARGET ${_real_target} POST_BUILD
                                COMMAND ${CMAKE_COMMAND} -E make_directory "C:/Program Files/Common Files/VST3/${AP_PROD_NAME}.vst3/resources/${_res_dir}"
                                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/resources/${_res}" "C:/Program Files/Common Files/VST3/${AP_PROD_NAME}.vst3/resources/${_res_dir}"
                                COMMENT "Copying resource ${_res} to installed VST3 resources for ${_real_target}"
                                VERBATIM)
                        else()
                            add_custom_command(TARGET ${_real_target} POST_BUILD
                                COMMAND ${CMAKE_COMMAND} -E make_directory "C:/Program Files/Common Files/VST3/${AP_PROD_NAME}.vst3/resources"
                                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/resources/${_res}" "C:/Program Files/Common Files/VST3/${AP_PROD_NAME}.vst3/resources"
                                COMMENT "Copying resource ${_res} to installed VST3 resources for ${_real_target}"
                                VERBATIM)
                        endif()
                    endforeach()
                endif()
            endif()
        endforeach()
    endif()
    
    # ===================== Install Rules for Packaging =====================
    # Install Standalone executables
    if(TARGET ${target}_Standalone)
        if(APPLE)
            install(TARGETS ${target}_Standalone
                    BUNDLE DESTINATION bin
                    COMPONENT ${target})
        else()
            install(TARGETS ${target}_Standalone
                    RUNTIME DESTINATION bin
                    COMPONENT ${target})
        endif()
    endif()
    
    # Install VST3 plugins
    if(TARGET ${target}_VST3)
        install(TARGETS ${target}_VST3
                LIBRARY DESTINATION vst3
                COMPONENT ${target})
    endif()
    
    # Install AU plugins (macOS only)
    if(TARGET ${target}_AU AND APPLE)
        install(TARGETS ${target}_AU
                LIBRARY DESTINATION au
                COMPONENT ${target})
    endif()
endfunction(add_plugin)

# ============================================================
# PLUGINS (Only build when this is the top-level project)
# ============================================================
if(PROJECT_IS_TOP_LEVEL)
    message(STATUS "Building example plugins...")
    

    # Scan all folders inside plugins/ and automatically add them as subdirectories (do not filter here)
    file(GLOB plugin_dirs RELATIVE ${CMAKE_SOURCE_DIR}/plugins ${CMAKE_SOURCE_DIR}/plugins/*)
    foreach(plugin_dir IN LISTS plugin_dirs)
        if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/${plugin_dir}")
            message(STATUS "Adding plugin directory: ${plugin_dir}")
            add_subdirectory(plugins/${plugin_dir})
        endif()
    endforeach()

    # Now filter for valid_plugin_dirs (for ALL_PLUGIN_TARGETS etc.)
    set(valid_plugin_dirs "")
    foreach(plugin_dir IN LISTS plugin_dirs)
        if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/plugins/${plugin_dir}")
            string(SUBSTRING "${plugin_dir}" 0 1 first_char)
            if(NOT first_char STREQUAL "." AND NOT first_char STREQUAL "_")
                list(APPEND valid_plugin_dirs "${plugin_dir}")
            endif()
        endif()
    endforeach()

    # Collect plugin targets for each format only from valid_plugin_dirs
    set(ALL_PLUGIN_TARGETS "")
    set(ALL_PLUGIN_AU_TARGETS "")
    set(ALL_PLUGIN_STANDALONE_TARGETS "")
    set(ALL_PLUGIN_VST3_TARGETS "")

    foreach(plugin_dir IN LISTS valid_plugin_dirs)
        string(REPLACE " " "" plugin_target_base "${plugin_dir}")
        list(APPEND ALL_PLUGIN_TARGETS "${plugin_target_base}_AU" "${plugin_target_base}_Standalone" "${plugin_target_base}_VST3")
        list(APPEND ALL_PLUGIN_AU_TARGETS "${plugin_target_base}_AU")
        list(APPEND ALL_PLUGIN_STANDALONE_TARGETS "${plugin_target_base}_Standalone")
        list(APPEND ALL_PLUGIN_VST3_TARGETS "${plugin_target_base}_VST3")
    endforeach()

    # Add custom targets for all plugins and each format
    add_custom_target(all_plugins_All_Formats DEPENDS ${ALL_PLUGIN_TARGETS})
    add_custom_target(all_plugins_AU DEPENDS ${ALL_PLUGIN_AU_TARGETS})
    add_custom_target(all_plugins_Standalone DEPENDS ${ALL_PLUGIN_STANDALONE_TARGETS})
    add_custom_target(all_plugins_VST3 DEPENDS ${ALL_PLUGIN_VST3_TARGETS})

    # ============================================================
    # DEMOS (Only build when this is the top-level project)
    # ============================================================

    # Scan all folders inside demos/ and automatically add them as subdirectories
    file(GLOB demo_dirs RELATIVE ${CMAKE_SOURCE_DIR}/demos ${CMAKE_SOURCE_DIR}/demos/*)

    foreach(demo_dir IN LISTS demo_dirs)
        if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/demos/${demo_dir}")
            message(STATUS "Adding demo directory: ${demo_dir}")
            add_subdirectory(demos/${demo_dir})
        endif()
    endforeach()
else()
    message(STATUS "Skipping example plugins and demos (not top-level project)")
endif()




