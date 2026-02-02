# JonssonicPlugins
A plugin development framework and collection of audio effects built with [JonssonicDSP](https://github.com/Ion3rik/JonssonicDSP), JUCE, and CMake. Includes a lightweight framework for rapid plugin development with automatic parameter management and GUI generation. 

## Example Plugins
- Delay, Flanger, Reverb, EQ, Compressor, Distortion.

## Framework

This project includes the **jnsc::juce_interface** - a lightweight framework that simplifies plugin development with JUCE by providing:

- **ParameterManager**: Type-safe parameter management with automatic APVTS integration and thread-safe updates
- **ControlPanel**: Automatic GUI generation from parameters with customizable layouts
- **CustomLookAndFeel**: Modern UI styling with knob graphics and consistent design (easily customizable by dropping your own knob PNGs in `resources/knobs/`)
- **Parameter Types**: Support for Float, Bool, and Choice parameters with built-in validation

The framework handles the boilerplate of parameter synchronization between GUI and audio threads, letting you focus on DSP implementation.



## Creating a New Plugin

Use the provided Python script to quickly init a new plugin:

```bash
# Create a plugin in the plugins/ directory
python3 tools/init_new_plugin.py PluginName

# Create a demo plugin in the demos/ directory
python3 tools/init_new_plugin.py PluginName --demo
```

**Examples:**
```bash
# Create a Chorus effect plugin
python3 tools/init_new_plugin.py Chorus

# Create a demo showcasing oversampling
python3 tools/init_new_plugin.py OversamplingDemo --demo
```

The script will:
- Create a new directory with your plugin name
- Copy and customize all template files (CMakeLists.txt, source files, headers)
- Replace all "Template" references with your plugin name
- The plugin will be automatically detected and built by CMake

**Next Steps After Creation:**
1. Customize the parameters in `YourPlugin/Params.h`
2. Implement your DSP logic in `YourPlugin/PluginProcessor.cpp`
3. Modify the GUI in `YourPlugin/PluginEditor.cpp` 
4. Build the plugin:
   ```bash
   # Configure CMake (if not already done)
   cmake --preset fetch-debug
   
   # Build the specific plugin
   cmake --build build/fetch-debug --target YourPluginName
   
   # Or build all plugins in the repo
   cmake --build build/fetch-debug
   ```
