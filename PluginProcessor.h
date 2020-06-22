
#ifndef PROCESSORH
#define PROCESSORH

#include <JuceHeader.h>


#define MEM_SIZE 50000
#define OPENHOLE_FREQ 349.0


//==============================================================================
/**
*/
class BirlOscillatorAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    BirlOscillatorAudioProcessor();
    ~BirlOscillatorAudioProcessor();
    double gain{};
    double frequency{};
    double filterfrequency{};
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    //==============================================================================
    tSVF filter1{};
    tSVF filter2{};
    float modAmount{10};

private:
    tSawtooth saw1{};
    tSawtooth saw2{};
    tSawtooth saw3{};
    
    tCycle sin{};
    tCycle mod{};

    tExpSmooth smoother{};
    
    // detune array should take MIDI
    float detune[3] = {0.999, 1.0, 1.001};
    
    
//    Array<tSawtooth> saws;
//    saws.add
    char myMemory[MEM_SIZE];
    float audioBufferTick{};
    
    //freq modulation
    float modTick{};
    
    float smoothGain{};
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BirlOscillatorAudioProcessor)
};

#endif /*PROCESSORH*/
