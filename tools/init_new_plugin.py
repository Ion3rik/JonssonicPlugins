#!/usr/bin/env python3
import os
import sys

if len(sys.argv) < 2:
    print("Usage: python tools/new_plugin.py PluginName")
    sys.exit(1)

PLUGIN_NAME = sys.argv[1]
PLUGIN_DIR = os.path.join("plugins", PLUGIN_NAME)

if os.path.exists(PLUGIN_DIR):
    print(f"Error: Plugin directory '{PLUGIN_DIR}' already exists.")
    sys.exit(1)

os.makedirs(PLUGIN_DIR)

def write_file(path, content):
    with open(path, "w", encoding="utf-8") as f:
        f.write(content)

# Class names for Processor and Editor
PROCESSOR_CLASS = f"{PLUGIN_NAME}AudioProcessor"
EDITOR_CLASS = f"{PLUGIN_NAME}AudioProcessorEditor"

# --- CMakeLists.txt ---
cmake_contents = f"""\
add_plugin({PLUGIN_NAME}
    TARGET {PLUGIN_NAME}
    VERSION 0.1.0
    PLUGIN_NAME "{PLUGIN_NAME}"
    PROD_NAME "{PLUGIN_NAME}"
    PROD_CODE "{PLUGIN_NAME[:4].upper()}"
    SYNTH FALSE
    SOURCES
        PluginProcessor.cpp
        PluginEditor.cpp
    INCLUDE_DIRS
)
"""

# --- PluginProcessor.h ---
processor_h = f"""\
#pragma once

#include <JuceHeader.h>

class {PROCESSOR_CLASS} : public juce::AudioProcessor
{{
public:
    {PROCESSOR_CLASS}();
    ~{PROCESSOR_CLASS}() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR({PROCESSOR_CLASS})
}};
"""

# --- PluginProcessor.cpp ---
processor_cpp = f"""\
#include "PluginProcessor.h"
#include "PluginEditor.h"

{PROCESSOR_CLASS}::{PROCESSOR_CLASS}()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                      )
#endif
{{
}}

{PROCESSOR_CLASS}::~{PROCESSOR_CLASS}() {{}}

const juce::String {PROCESSOR_CLASS}::getName() const {{ return JucePlugin_Name; }}
bool {PROCESSOR_CLASS}::acceptsMidi() const {{ return false; }}
bool {PROCESSOR_CLASS}::producesMidi() const {{ return false; }}
bool {PROCESSOR_CLASS}::isMidiEffect() const {{ return false; }}
double {PROCESSOR_CLASS}::getTailLengthSeconds() const {{ return 0.0; }}

int {PROCESSOR_CLASS}::getNumPrograms() {{ return 1; }}
int {PROCESSOR_CLASS}::getCurrentProgram() {{ return 0; }}
void {PROCESSOR_CLASS}::setCurrentProgram (int) {{}}
const juce::String {PROCESSOR_CLASS}::getProgramName (int) {{ return {{}}; }}
void {PROCESSOR_CLASS}::changeProgramName (int, const juce::String&) {{}}

void {PROCESSOR_CLASS}::prepareToPlay (double sampleRate, int samplesPerBlock)
{{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}}

void {PROCESSOR_CLASS}::releaseResources()
{{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}}

void {PROCESSOR_CLASS}::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, clear unused channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Main processing loop here
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {{
        auto* channelData = buffer.getWritePointer (channel);

        // Your DSP code here, currently passthrough
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            channelData[sample] = channelData[sample];
    }}
}}

juce::AudioProcessorEditor* {PROCESSOR_CLASS}::createEditor()
{{
    return new {EDITOR_CLASS} (*this);
}}

bool {PROCESSOR_CLASS}::hasEditor() const {{ return true; }}

void {PROCESSOR_CLASS}::getStateInformation (juce::MemoryBlock& destData)
{{
    // You should use this method to store your parameters in the memory block.
}}

void {PROCESSOR_CLASS}::setStateInformation (const void* data, int sizeInBytes)
{{
    // You should use this method to restore your parameters from this memory block,
}}
"""

# --- PluginEditor.h ---
editor_h = f"""\
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class {EDITOR_CLASS} : public juce::AudioProcessorEditor
{{
public:
    {EDITOR_CLASS} ({PROCESSOR_CLASS}&);
    ~{EDITOR_CLASS}() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    {PROCESSOR_CLASS}& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR ({EDITOR_CLASS})
}};
"""

# --- PluginEditor.cpp ---
editor_cpp = f"""\
#include "PluginEditor.h"

{EDITOR_CLASS}::{EDITOR_CLASS} ({PROCESSOR_CLASS}& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{{
    setSize (400, 300);
}}

{EDITOR_CLASS}::~{EDITOR_CLASS}() {{}}

void {EDITOR_CLASS}::paint (juce::Graphics& g)
{{
    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("{PLUGIN_NAME}", getLocalBounds(), juce::Justification::centred, 1);
}}

void {EDITOR_CLASS}::resized()
{{
    // This is where you would lay out the positions of any
    // subcomponents in your editor..
}}
"""

# --- README.md ---
readme_md = f"""\
# {PLUGIN_NAME}

This is the {PLUGIN_NAME} audio plugin.

- Version: 0.1.0
- Author: Your Name
- License: MIT

Build this plugin as part of the JonssonicPlugins project.

"""

# Write all files
write_file(os.path.join(PLUGIN_DIR, "CMakeLists.txt"), cmake_contents)
write_file(os.path.join(PLUGIN_DIR, "PluginProcessor.h"), processor_h)
write_file(os.path.join(PLUGIN_DIR, "PluginProcessor.cpp"), processor_cpp)
write_file(os.path.join(PLUGIN_DIR, "PluginEditor.h"), editor_h)
write_file(os.path.join(PLUGIN_DIR, "PluginEditor.cpp"), editor_cpp)
write_file(os.path.join(PLUGIN_DIR, "README.md"), readme_md)

print(f"Plugin '{PLUGIN_NAME}' created successfully in {PLUGIN_DIR}/")
print("You can now add it to your main build or it should be auto-detected if you use the existing scanning logic.")
 