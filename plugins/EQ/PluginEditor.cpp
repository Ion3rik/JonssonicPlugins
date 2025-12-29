#include "PluginEditor.h"
#include <gui/CustomLookAndFeel.h>

EQAudioProcessorEditor::EQAudioProcessorEditor(EQAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p),
      controlPanelConfig([]{
        Jonssonic::ControlPanelConfig c;
        c.columns = 3; // Number of columns in the control panel
        c.showValueBoxes = true; // Show value boxes for sliders
        c.title = "JONSSONIC"; // Plugin title
        c.subtitle = "EQ"; // Plugin subtitle
        c.gradientBaseColour = juce::Colour(0xff2a5b3d).brighter(0.1f); 
          return c;
      }()),
      controlPanel(audioProcessor.getAPVTS(), controlPanelConfig)
{
    customLookAndFeel = std::make_unique<Jonssonic::EQLookAndFeel>(&controlPanelConfig);
    setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(controlPanel); // Add and make the control panel visible in the editor
    setSize (400, 350); // Set the size of the editor window in pixels
}

EQAudioProcessorEditor::~EQAudioProcessorEditor()
{
    setLookAndFeel(nullptr); // Reset the look and feel to default
    customLookAndFeel.reset();
}

//==============================================================================
void EQAudioProcessorEditor::paint(juce::Graphics& g)
{
    if (auto* laf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
        laf->drawCachedMainBackground(g);
    else
        g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void EQAudioProcessorEditor::resized()
{
    controlPanel.setBounds(getLocalBounds()); // Make the control panel fill the entire editor area
    if (auto* laf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
        laf->generateMainBackground(getWidth(), getHeight());
}
