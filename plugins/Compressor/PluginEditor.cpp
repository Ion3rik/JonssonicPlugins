#include "PluginEditor.h"
#include <gui/CustomLookAndFeel.h>

CompressorAudioProcessorEditor::CompressorAudioProcessorEditor(CompressorAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p),
            controlPanelConfig([]{
                Jonssonic::ControlPanelConfig c;
                c.columns = 3; // Number of columns in the control panel
                c.panelMarginRight = 50; // Extra right margin for meter
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
    addAndMakeVisible(controlPanel);

    // Add gain reduction meter, wiring to the state from the processor's visualizer manager
    if (const auto* variant = audioProcessor.getVisualizerManager().getState(CompressorVisualizers::ID::GainReduction)) {
        if (auto state = std::get_if<std::shared_ptr<Jonssonic::GainReductionMeterState>>(variant)) {
            gainReductionMeter = std::make_unique<Jonssonic::GainReductionMeterComponent>(*state);
            addAndMakeVisible(gainReductionMeter.get());
        }
    }

    setSize (400, 350); // Slightly wider to fit meter
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
        laf->drawCachedMainBackground(g);
    else
        g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void CompressorAudioProcessorEditor::resized()
{
    // Layout the meter on the right
    auto bounds = getLocalBounds();
    int meterWidth = 70;
    if (gainReductionMeter)
    {
    auto meterBounds = bounds; // make a copy to retain original bounds for control panel
    gainReductionMeter->setBounds(meterBounds.removeFromRight(meterWidth).reduced(15, 64)
);

    }

    // Control panel fills the rest
    controlPanel.setBounds(bounds);

    if (auto* laf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
        laf->generateMainBackground(getWidth(), getHeight());
}
