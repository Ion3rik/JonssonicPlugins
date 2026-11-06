#include "PluginEditor.h"

PhaserAudioProcessorEditor::PhaserAudioProcessorEditor(PhaserAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p), controlPanelConfig([] {
          jnsc::juce_interface::ControlPanelConfig c;
          c.version = JucePlugin_VersionString;
          c.columns = 3;           // Number of columns in the control panel
          c.showValueBoxes = true; // Show value boxes for sliders
          c.title = "JONSSONIC";   // Plugin title
          c.subtitle = "PHASER";  // Plugin subtitle
          c.gradientBaseColour = juce::Colour(0xff8A45C6).brighter(0.1f);

          // Optionally set other config fields here

          return c;
      }()),
      controlPanel(audioProcessor.getAPVTS(), controlPanelConfig) {
    customLookAndFeel = std::make_unique<PhaserLookAndFeel>(&controlPanelConfig);
    setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(controlPanel);
    setSize(400, 350);
}

PhaserAudioProcessorEditor::~PhaserAudioProcessorEditor() {
    setLookAndFeel(nullptr); // Reset the look and feel to default
    customLookAndFeel.reset();
}
#include <gui/CustomLookAndFeel.h>

//==============================================================================
void PhaserAudioProcessorEditor::paint(juce::Graphics& g) {
    if (auto* laf = dynamic_cast<PhaserLookAndFeel*>(&getLookAndFeel()))
        laf->drawCachedMainBackground(g);
    else
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PhaserAudioProcessorEditor::resized() {
    controlPanel.setBounds(getLocalBounds());
    if (auto* laf = dynamic_cast<PhaserLookAndFeel*>(&getLookAndFeel()))
        laf->generateMainBackground(getWidth(), getHeight());
}
