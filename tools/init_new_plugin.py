#!/usr/bin/env python3
"""
Plugin Generator Script
Generates a new plugin from the _Template folder.

Usage:
    python tools/init_new_plugin.py PluginName [--demo]
    
Arguments:
    PluginName: Name of the plugin (e.g., HardClipper, Chorus)
    --demo:     Optional flag to create plugin in demos/ instead of plugins/

Examples:
    python tools/init_new_plugin.py Chorus
    python tools/init_new_plugin.py HardClipper --demo
"""

import os
import sys
import shutil
from pathlib import Path

def print_usage():
    print("Usage: python tools/init_new_plugin.py PluginName [--demo]")
    print("  PluginName: Name of the plugin (e.g., HardClipper, Chorus)")
    print("  --demo:     Create plugin in demos/ instead of plugins/")
    sys.exit(1)

def replace_template_names(content, plugin_name):
    """Replace all variations of 'Template' with the plugin name."""
    # Replace TemplateAudioProcessor → PluginNameAudioProcessor
    content = content.replace("TemplateAudioProcessor", f"{plugin_name}AudioProcessor")
    
    # Replace TemplateAudioProcessorEditor → PluginNameAudioProcessorEditor
    content = content.replace("TemplateAudioProcessorEditor", f"{plugin_name}AudioProcessorEditor")
    
    # Replace TemplateParams → PluginNameParams
    content = content.replace("TemplateParams", f"{plugin_name}Params")
    
    # Replace PROD_CODE TEMP with unique 4-character code based on plugin name
    prod_code = plugin_name[:4].upper() if len(plugin_name) >= 4 else plugin_name.upper().ljust(4, 'X')
    content = content.replace("PROD_CODE TEMP", f"PROD_CODE {prod_code}")

    # Replace c.subtitle = "TEMPLATE"; with plugin name in uppercase
    content = content.replace('c.subtitle = "TEMPLATE";', f'c.subtitle = "{plugin_name.upper()}";')
    
    # Replace TemplateResources and TemplateBinaryData for plugin-specific resources and plugin CMakeLists.txt
    content = content.replace("TemplateResources", f"{plugin_name}Resources")
    content = content.replace("TemplateBinaryData", f"{plugin_name}BinaryData")
    # Also replace Template in add_subdirectory/resources/target_link_libraries in plugin CMakeLists.txt
    content = content.replace('target_link_libraries(Template', f'target_link_libraries({plugin_name}')
    content = content.replace('add_subdirectory(resources)', 'add_subdirectory(resources)')  # for clarity, but not strictly needed
    # Replace standalone "Template" with plugin name (for CMakeLists, comments, etc.)
    # Be careful with word boundaries to avoid replacing parts of words
    import re
    # Match "Template" as a whole word (with word boundaries)
    content = re.sub(r'\bTemplate\b', plugin_name, content)
    
    return content

def copy_and_transform_template(plugin_name, target_dir):
    """Copy template files and replace all Template references."""
    script_dir = Path(__file__).parent
    repo_root = script_dir.parent
    template_dir = repo_root / "plugins" / "_Template"
    
    if not template_dir.exists():
        print(f"Error: Template directory not found at {template_dir}")
        sys.exit(1)
    
    # Create target directory
    plugin_dir = repo_root / target_dir / plugin_name
    if plugin_dir.exists():
        print(f"Error: Plugin directory '{plugin_dir}' already exists.")
        sys.exit(1)
    
    plugin_dir.mkdir(parents=True, exist_ok=True)
    
    # List of files to copy and transform
    template_files = [
        "CMakeLists.txt",
        "Params.h",
        "PluginProcessor.h",
        "PluginProcessor.cpp",
        "PluginEditor.h",
        "PluginEditor.cpp",
        "PluginLookAndFeel.h",
        "resources/CMakeLists.txt"
    ]
    
    print(f"Creating plugin '{plugin_name}' in '{target_dir}/{plugin_name}/'...")
    
    for file_name in template_files:
        if file_name == "resources/CMakeLists.txt":
            source_file = template_dir / "resources" / "CMakeLists.txt"
            target_file = plugin_dir / "resources" / "CMakeLists.txt"
            target_file.parent.mkdir(parents=True, exist_ok=True)
        else:
            source_file = template_dir / file_name
            target_file = plugin_dir / file_name
        
        if not source_file.exists():
            print(f"Warning: Template file '{file_name}' not found, skipping...")
            continue
        
        # Read template file
        with open(source_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Transform content
        content = replace_template_names(content, plugin_name)
        
        # Write to target
        with open(target_file, 'w', encoding='utf-8') as f:
            f.write(content)
        
        print(f"  ✓ Created {file_name}")
    
    print(f"\n✅ Plugin '{plugin_name}' created successfully!")
    print(f"📁 Location: {target_dir}/{plugin_name}/")
    print(f"\nNext steps:")
    print(f"  1. Add 'add_subdirectory({plugin_name})' to {target_dir}/CMakeLists.txt")
    print(f"  2. Customize the parameters in {plugin_name}/Params.h")
    print(f"  3. Implement your DSP logic in {plugin_name}/PluginProcessor.cpp")

def main():
    # Parse arguments
    if len(sys.argv) < 2:
        print_usage()
    
    plugin_name = sys.argv[1]
    is_demo = "--demo" in sys.argv
    
    # Validate plugin name
    if not plugin_name.replace("_", "").isalnum():
        print("Error: Plugin name must be alphanumeric (underscores allowed)")
        sys.exit(1)
    
    # Determine target directory
    target_dir = "demos" if is_demo else "plugins"
    
    # Copy and transform template
    copy_and_transform_template(plugin_name, target_dir)

if __name__ == "__main__":
    main()
 