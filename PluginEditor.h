/*
 *
 *  Interface for rule-based Birl.
 *
 */

#ifndef EDITORH
#define EDITORH

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RuleBasedInterface.h"
#include "Synth.h"

#include "math.h"

//==============================================================================
/**
*/
class BirlOscillatorAudioProcessorEditor  : public AudioProcessorEditor

{
public:
    BirlOscillatorAudioProcessorEditor (BirlOscillatorAudioProcessor&);
    ~BirlOscillatorAudioProcessorEditor();

    //==========================================================================
    void paint (Graphics&) override;
    void resized() override;
//==============================================================================

private:
    RuleBasedInterface rbi;

//==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BirlOscillatorAudioProcessorEditor)
};

#endif /*EDITORH*/


