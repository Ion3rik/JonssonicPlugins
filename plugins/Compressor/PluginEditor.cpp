#include "PluginEditor.h"
#include <gui/CustomLookAndFeel.h>

CompressorAudioProcessorEditor::CompressorAudioProcessorEditor(CompressorAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p),
      controlPanelConfig([]{
        Jonssonic::ControlPanelConfig c;
        c.columns = 3; // Number of columns in the control panel
        c.showValueBoxes = true; // Show value boxes for sliders
        c.title = "JONSSONIC"; // Plugin title
        c.subtitle = "COMPRESSOR"; // Plugin subtitle
        c.gradientBaseColour = juce::Colour(0xffb36a1d).brighter(0.1f);
        // Optionally set other config fields here
          return c;
      }()),
      controlPanel(audioProcessor.getAPVTS(), controlPanelConfig)
{
    customLookAndFeel = std::make_unique<Jonssonic::CompressorLookAndFeel>(&controlPanelConfig);
    setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(controlPanel); // Add and make the control panel visible in the editor
    setSize (400, 350); // Set the size of the editor window in pixels
}

CompressorAudioProcessorEditor::~CompressorAudioProcessorEditor()
{
    setLookAndFeel(nullptr); // Reset the look and feel to default
    customLookAndFeel.reset();
}

//==============================================================================
void CompressorAudioProcessorEditor::paint(juce::Graphics& g)
{
    if (auto* laf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
        laf->drawMainBackground(g, getWidth(), getHeight());
    else
        g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void CompressorAudioProcessorEditor::resized()
{
    controlPanel.setBounds(getLocalBounds()); // Make the control panel fill the entire editor area
}
