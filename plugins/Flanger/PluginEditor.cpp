#include "PluginEditor.h"

FlangerAudioProcessorEditor::FlangerAudioProcessorEditor(FlangerAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p),
      controlPanelConfig([]{
          Jonssonic::ControlPanelConfig c;
          c.columns = 3;
          c.showValueBoxes = true;
          c.controlHeight = 100;
          // Optionally set other config fields here
          return c;
      }()),
      controlPanel(audioProcessor.getAPVTS(), controlPanelConfig)
{
    customLookAndFeel = std::make_unique<CustomLookAndFeel>();
    setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(controlPanel);
    setSize (400, 300);
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
    CustomLookAndFeel::drawMainBackground(g, getWidth(), getHeight());
}

void FlangerAudioProcessorEditor::resized()
{
    controlPanel.setBounds(getLocalBounds());
}
