#include "PluginEditor.h"

BiquadDemoAudioProcessorEditor::BiquadDemoAudioProcessorEditor(BiquadDemoAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p),
      controlPanelConfig([]{
          Jonssonic::ControlPanelConfig c;
          c.columns = 3; // Number of columns in the control panel
          c.showValueBoxes = true; // Show value boxes for sliders
          c.controlHeight = 100; // Height of each control in pixels
          // Optionally set other config fields here
          return c;
      }()),
      controlPanel(audioProcessor.getAPVTS(), controlPanelConfig)
{
    customLookAndFeel = std::make_unique<CustomLookAndFeel>();
    setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(controlPanel); // Add and make the control panel visible in the editor
    setSize (400, 250); // Set the size of the editor window in pixels
}

BiquadDemoAudioProcessorEditor::~BiquadDemoAudioProcessorEditor()
{
    setLookAndFeel(nullptr); // Reset the look and feel to default
    customLookAndFeel.reset();
}
#include <gui/CustomLookAndFeel.h>

//==============================================================================
void BiquadDemoAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void BiquadDemoAudioProcessorEditor::resized()
{
    controlPanel.setBounds(getLocalBounds()); // Make the control panel fill the entire editor area
}
