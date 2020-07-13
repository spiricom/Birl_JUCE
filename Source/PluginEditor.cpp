#include "PluginEditor.h"
#include "PluginProcessor.h"

BirlOneHoleAudioProcessorEditor::BirlOneHoleAudioProcessorEditor (BirlOneHoleAudioProcessor& p)
    : AudioProcessorEditor (&p), processor(p), slidersAndKeys(p)
{
    setSize (500, 300);
    
    slidersAndKeys.setBounds(0, 0, 500, 300);
    addAndMakeVisible(slidersAndKeys);
    
//    animate.setBounds(180, 180, 80, 80);
//    addAndMakeVisible(animate);
    
}

BirlOneHoleAudioProcessorEditor::~BirlOneHoleAudioProcessorEditor()
{
}

void BirlOneHoleAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::palegreen);
    g.setColour(Colours::white);
    g.setFont(18.0f);

}

void BirlOneHoleAudioProcessorEditor::resized()
{
}
