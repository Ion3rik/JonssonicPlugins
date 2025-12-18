#include "PluginEditor.h"

FlangerAudioProcessorEditor::FlangerAudioProcessorEditor(FlangerAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p),
      controlPanelConfig([]{
        Jonssonic::ControlPanelConfig c;
        c.columns = 3; // Number of columns in the control panel
        c.showValueBoxes = true; // Show value boxes for sliders
        c.controlHeight = 80; // Height of each control in pixels
        c.labelHeight = 20; // Height of labels in pixels
        c.spacing = 0; // Spacing between controls in pixels

        c.title = "JONSSONIC"; // Plugin title
        c.subtitle = "FLANGER"; // Plugin subtitle
        c.gradientBaseColour = juce::Colour(0xff5b2a4a);
        
      
        // Optionally set other config fields here

          return c;
      }()),
      controlPanel(audioProcessor.getAPVTS(), controlPanelConfig)
{
    customLookAndFeel = std::make_unique<CustomLookAndFeel>(&controlPanelConfig);
    setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(controlPanel);
    setSize (400, 350);
}

FlangerAudioProcessorEditor::~FlangerAudioProcessorEditor()
{
    setLookAndFeel(nullptr); // Reset the look and feel to default
    customLookAndFeel.reset();
}
#include <gui/CustomLookAndFeel.h>

//==============================================================================
void FlangerAudioProcessorEditor::paint(juce::Graphics& g)
{
    if (auto* laf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
        laf->drawMainBackground(g, getWidth(), getHeight());
    else
        g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void FlangerAudioProcessorEditor::resized()
{
    controlPanel.setBounds(getLocalBounds());
}
