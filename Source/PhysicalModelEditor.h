#ifndef PHYSICAL_MODEL_EDITOR_H
#define PHYSICAL_MODEL_EDITOR_H

#include <JuceHeader.h>
#include <math.h>
#include "PhysicalModel.h"


class PhysicalModelEditor : public Component
{
public:
    PhysicalModelEditor()
    {
        addAndMakeVisible(connectButton);
        connectButton.setBounds(400, 20, 80, 30);
        
        for(int i = 0; i < 9; ++i){
            keySliders[i].setSliderStyle(Slider::LinearVertical);
            keySliders[i].setRange(0.0, 1.0);
            keySliders[i].setBounds(100+30*i, 30, 20, 100);
            keySliders[i].setNumDecimalPlacesToDisplay(0);
            addAndMakeVisible(keySliders[i]);
        }
        
        midiAdjustmentLabel.setBounds(0, 200, 100, 100);
        addAndMakeVisible(midiAdjustmentLabel);
        
        frequencyLabel.setBounds(0, 230, 100, 100);
        addAndMakeVisible(frequencyLabel);
        
        setSize (320, 200);
    }
    ~PhysicalModelEditor(){}

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);
    }

    void resized() override
    {
    }

protected:
    Slider keySliders[NUM_OF_KEYS];
    TextButton buttonButtons[NUM_OF_KEYS];
    TextButton connectButton {"Connect"};
    Label midiAdjustmentLabel;
    Label frequencyLabel;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhysicalModelEditor)
};
#endif
