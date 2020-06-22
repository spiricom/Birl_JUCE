
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
BirlOscillatorAudioProcessorEditor::BirlOscillatorAudioProcessorEditor (BirlOscillatorAudioProcessor& p)
    : AudioProcessorEditor (&p), rbi(p)
{
    setSize (500, 500);
    rbi.setBounds(0, 0, 500, 400);
    addAndMakeVisible(rbi);

}

BirlOscillatorAudioProcessorEditor::~BirlOscillatorAudioProcessorEditor()
{
}

//==============================================================================
void BirlOscillatorAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::black);
    g.setColour(Colours::white);
    g.setFont(15.0f);
}

void BirlOscillatorAudioProcessorEditor::resized()
{
}



