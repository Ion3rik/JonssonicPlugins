#include "PluginEditor.h"
#include <gui/CustomLookAndFeel.h>

ReverbAudioProcessorEditor::ReverbAudioProcessorEditor(ReverbAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p), controlPanelConfig([] {
          jnsc::juce_interface::ControlPanelConfig c;
          c.columns = 3;           // Number of columns in the control panel
          c.showValueBoxes = true; // Show value boxes for sliders
          c.title = "JONSSONIC";   // Plugin title
          c.subtitle = "REVERB";   // Plugin subtitle
          c.gradientBaseColour = juce::Colour(0xff3d2a5b).brighter(0.1f);
          // Optionally set other config fields here
          return c;
      }()),
      controlPanel(audioProcessor.getAPVTS(), controlPanelConfig) {
    customLookAndFeel = std::make_unique<ReverbLookAndFeel>(&controlPanelConfig);
    setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(controlPanel); // Add and make the control panel visible in the editor
    setSize(400, 450);               // Set the size of the editor window in pixels
}

ReverbAudioProcessorEditor::~ReverbAudioProcessorEditor() {
    setLookAndFeel(nullptr); // Reset the look and feel to default
    customLookAndFeel.reset();
}

//==============================================================================
void ReverbAudioProcessorEditor::paint(juce::Graphics& g) {
    if (auto* laf = dynamic_cast<ReverbLookAndFeel*>(&getLookAndFeel()))
        laf->drawCachedMainBackground(g);
    else
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void ReverbAudioProcessorEditor::resized() {
    controlPanel.setBounds(getLocalBounds()); // Make the control panel fill the entire editor area
    if (auto* laf = dynamic_cast<ReverbLookAndFeel*>(&getLookAndFeel()))
        laf->generateMainBackground(getWidth(), getHeight());
}
