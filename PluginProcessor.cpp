
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "RuleBasedInterface.h"


//==============================================================================
BirlOscillatorAudioProcessor::BirlOscillatorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

}

BirlOscillatorAudioProcessor::~BirlOscillatorAudioProcessor()
{
    //body
}

//==============================================================================
const String BirlOscillatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BirlOscillatorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BirlOscillatorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BirlOscillatorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BirlOscillatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BirlOscillatorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BirlOscillatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BirlOscillatorAudioProcessor::setCurrentProgram (int index)
{
}

const String BirlOscillatorAudioProcessor::getProgramName (int index)
{
    return {};
}

void BirlOscillatorAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void BirlOscillatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    LEAF_init(sampleRate, samplesPerBlock, myMemory, 500000, []() {return (float)rand() / RAND_MAX; });
    tCycle_init(&sin);
    tCycle_init(&mod);

    tSawtooth_init(&saw1);
    tSawtooth_init(&saw2);
    tSawtooth_init(&saw3);
    tSVF_init(&filter1, SVFTypeLowpass, frequency, 0.7f);
    tSVF_init(&filter2, SVFTypeLowpass, frequency, 0.7f);

    tExpSmooth_init(&smoother, 0.0, 0.01); // 0.01 = 10ms


}

void BirlOscillatorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BirlOscillatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BirlOscillatorAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{

//    buffer.clear();
    
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    Array<float*> channelData;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        channelData.add(buffer.getWritePointer (channel));
    }
    
    for (int i = 0; i < buffer.getNumSamples(); ++i) {
        
        // SMOOTHER
        tExpSmooth_setDest(&smoother, gain);
        smoothGain = tExpSmooth_tick(&smoother);
        
        // FREQ MODULATION
        tCycle_setFreq(&mod, frequency*2);

        modTick = tCycle_tick(&mod) * modAmount;
        
    
        tSawtooth_setFreq(&saw1, frequency*detune[0]);
        tSawtooth_setFreq(&saw2, frequency*detune[1]);
        tSawtooth_setFreq(&saw3, frequency*detune[2]);
        
        audioBufferTick = tSawtooth_tick(&saw1)
                        + tSawtooth_tick(&saw2)
                        + tSawtooth_tick(&saw3);
        
        filterfrequency = LEAF_clip(30.0, smoothGain
                         * 15 * frequency, 17000.0);
        
        tSVF_setFreq(&filter1, filterfrequency);
        tSVF_setFreq(&filter2, filterfrequency);
        
        audioBufferTick = tSVF_tick(&filter1, audioBufferTick);
        audioBufferTick = tSVF_tick(&filter2, audioBufferTick);
        
        tCycle_setFreq(&sin, frequency + modTick);
        
        audioBufferTick += tCycle_tick(&sin);
        
        audioBufferTick *= smoothGain * 0.25;
        


        
        for (int j = 0; j < totalNumInputChannels; ++j) {
            channelData.getUnchecked(j)[i] = audioBufferTick;
        }
    }
        
}


//==============================================================================
bool BirlOscillatorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* BirlOscillatorAudioProcessor::createEditor()
{
    return new BirlOscillatorAudioProcessorEditor (*this);
}

//==============================================================================
void BirlOscillatorAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BirlOscillatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BirlOscillatorAudioProcessor();
}
