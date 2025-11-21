#include "PluginEditor.h"

FlangerAudioProcessorEditor::FlangerAudioProcessorEditor(FlangerAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p)
    
{

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
   
}
