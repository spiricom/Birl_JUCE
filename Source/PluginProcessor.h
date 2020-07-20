#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <JuceHeader.h>
#include <algorithm>

#include "PhysicalModel.h"

#define MEM_SIZE 50000
#define OPENHOLE_FREQ 440.0

class BirlOneHoleAudioProcessor  : public AudioProcessor, public PhysicalModel
{
public:
    BirlOneHoleAudioProcessor();
    ~BirlOneHoleAudioProcessor();
    
    double gain;
    double octave;
    double frequency = OPENHOLE_FREQ;
        
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    //==============================================================================
    void synthesize();
    void parameterSmooth();
    void buttonListen();
    //==============================================================================
    tSVF filter1{};
    tSVF filter2{};
    float modAmount{10};

private:
    PhysicalModel birl;
    /* Envelopes */
    tADSR breathPosEnv;
    
    tExpSmooth XY1_XEnv;
    tExpSmooth XY1_YEnv;
    tExpSmooth XY2_XEnv;
    tExpSmooth XY2_YEnv;
    tExpSmooth keysEnv;

//    tSawtooth saw1;
//    tSawtooth saw2;
//    tSawtooth saw3;
    
    tCycle sin{};
    tCycle mod{};

    tExpSmooth smoother{};
    
    // detune array should take MIDI
    float detune[3] = {0.999, 1.0, 1.001};
    
    

    char myMemory[MEM_SIZE];
    float audioBufferTick{};
    
    //freq modulation
    float modTick;
    
    float smoothGain;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BirlOneHoleAudioProcessor)
};

#endif

