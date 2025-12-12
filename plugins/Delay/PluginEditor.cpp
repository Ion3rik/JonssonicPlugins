#include "PluginEditor.h"
#include <gui/CustomLookAndFeel.h>

DelayAudioProcessorEditor::DelayAudioProcessorEditor(DelayAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p),
      controlPanelConfig([]{
          Jonssonic::ControlPanelConfig c;
          c.columns = 3; // Number of columns in the control panel
          c.showValueBoxes = true; // Show value boxes for sliders
          c.controlHeight = 80; // Height of each control in pixels
          c.labelHeight = 20; // Height of labels in pixels
          c.spacing = 10; // Spacing between controls in pixels
          c.title = "JONSSONIC DELAY"; // Plugin title
          c.titleHeight = 60; // Height of title area
          c.fontName = "Avenir"; // Font name for labels and title
          // Optionally set other config fields here
          return c;
      }()),
      controlPanel(audioProcessor.getAPVTS(), controlPanelConfig)
{
    customLookAndFeel = std::make_unique<CustomLookAndFeel>();
    setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(controlPanel); // Add and make the control panel visible in the editor
    setSize (400, 350); // Set the size of the editor window in pixels
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
    setLookAndFeel(nullptr); // Reset the look and feel to default
    customLookAndFeel.reset();
}

//==============================================================================
void DelayAudioProcessorEditor::paint(juce::Graphics& g)
{
    if (auto* laf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
        laf->drawMainBackground(g, getWidth(), getHeight(), &controlPanelConfig);
    else
        g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void DelayAudioProcessorEditor::resized()
{
    controlPanel.setBounds(getLocalBounds()); // Make the control panel fill the entire editor area
}
