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
    addAndMakeVisible(controlPanel);
    setSize (400, 250);
}

FlangerAudioProcessorEditor::~FlangerAudioProcessorEditor()
{
    setLookAndFeel(nullptr); // Reset the look and feel to default
}

//==============================================================================
void FlangerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void FlangerAudioProcessorEditor::resized()
{
    controlPanel.setBounds(getLocalBounds());
}
