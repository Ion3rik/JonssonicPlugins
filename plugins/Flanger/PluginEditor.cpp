#include "PluginEditor.h"


MyAudioProcessorEditor::MyAudioProcessorEditor(MyAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p),
    genericParamEditor(audioProcessor.getParameterManager())
    
{

}



MyAudioProcessorEditor::~MyAudioProcessorEditor()
{
    setLookAndFeel(nullptr); // Reset the look and feel to default
}

//==============================================================================
void MyAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MyAudioProcessorEditor::resized()
{
   
}
