#include "PluginEditor.h"
#include <gui/CustomLookAndFeel.h>

DistortionAudioProcessorEditor::DistortionAudioProcessorEditor(DistortionAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p), controlPanelConfig([] {
          jnsc::juce_interface::ControlPanelConfig c;
          c.columns = 3;             // Number of columns in the control panel
          c.showValueBoxes = true;   // Show value boxes for sliders
          c.title = "JONSSONIC";     // Plugin title
          c.subtitle = "DISTORTION"; // Plugin subtitle
          c.gradientBaseColour = juce::Colour(0xff8c1d1d).brighter(0.1f);
          // Optionally set other config fields here
          return c;
      }()),
      controlPanel(audioProcessor.getAPVTS(), controlPanelConfig) {
    customLookAndFeel = std::make_unique<DistortionLookAndFeel>(&controlPanelConfig);
    setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(controlPanel); // Add and make the control panel visible in the editor
    setSize(400, 350);               // Set the size of the editor window in pixels
}

DistortionAudioProcessorEditor::~DistortionAudioProcessorEditor() {
    setLookAndFeel(nullptr); // Reset the look and feel to default
    customLookAndFeel.reset();
}

//==============================================================================
void DistortionAudioProcessorEditor::paint(juce::Graphics& g) {
    if (auto* laf = dynamic_cast<jnsc::juce_interface::CustomLookAndFeel*>(&getLookAndFeel()))
        laf->drawCachedMainBackground(g);
    else
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void DistortionAudioProcessorEditor::resized() {
    controlPanel.setBounds(getLocalBounds()); // Make the control panel fill the entire editor area
    if (auto* laf = dynamic_cast<jnsc::juce_interface::CustomLookAndFeel*>(&getLookAndFeel()))
        laf->generateMainBackground(getWidth(), getHeight());
}
