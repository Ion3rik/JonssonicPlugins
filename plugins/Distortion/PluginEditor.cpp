#include "PluginEditor.h"
#include <gui/CustomLookAndFeel.h>

DistortionAudioProcessorEditor::DistortionAudioProcessorEditor(DistortionAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p),
      controlPanelConfig([]{
        Jonssonic::ControlPanelConfig c;
        c.columns = 3; // Number of columns in the control panel
        c.showValueBoxes = true; // Show value boxes for sliders
        c.controlHeight = 80; // Height of each control in pixels
        c.labelHeight = 20; // Height of labels in pixels
        c.spacing = 0; // Spacing between controls in pixels
        c.title = "JONSSONIC"; // Plugin title
        c.subtitle = "DISTORTION"; // Plugin subtitle
        c.gradientBaseColour = juce::Colour(0xff8c1d1d);
        // Optionally set other config fields here
          return c;
      }()),
      controlPanel(audioProcessor.getAPVTS(), controlPanelConfig)
{
    customLookAndFeel = std::make_unique<CustomLookAndFeel>(&controlPanelConfig);
    setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(controlPanel); // Add and make the control panel visible in the editor
    setSize (400, 350); // Set the size of the editor window in pixels
}

DistortionAudioProcessorEditor::~DistortionAudioProcessorEditor()
{
    setLookAndFeel(nullptr); // Reset the look and feel to default
    customLookAndFeel.reset();
}

//==============================================================================
void DistortionAudioProcessorEditor::paint(juce::Graphics& g)
{
    if (auto* laf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
        laf->drawMainBackground(g, getWidth(), getHeight());
    else
        g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void DistortionAudioProcessorEditor::resized()
{
    controlPanel.setBounds(getLocalBounds()); // Make the control panel fill the entire editor area
}
