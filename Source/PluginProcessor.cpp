#include "PluginProcessor.h"
#include "PluginEditor.h"

MIDIFXAudioProcessor::MIDIFXAudioProcessor()
    : AudioProcessor (BusesProperties())
{
}

MIDIFXAudioProcessor::~MIDIFXAudioProcessor()
{
}

const juce::String MIDIFXAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MIDIFXAudioProcessor::acceptsMidi() const
{
    return true;
}

bool MIDIFXAudioProcessor::producesMidi() const
{
    return true;
}

bool MIDIFXAudioProcessor::isMidiEffect() const
{
    return true;
}

double MIDIFXAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MIDIFXAudioProcessor::getNumPrograms()
{
    return 1;
}

int MIDIFXAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MIDIFXAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MIDIFXAudioProcessor::getProgramName (int index)
{
    return {};
}

void MIDIFXAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void MIDIFXAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void MIDIFXAudioProcessor::releaseResources()
{
}

void MIDIFXAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    
    // This is where we'll process MIDI messages
    // For now, we're just passing through all MIDI messages
}

bool MIDIFXAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* MIDIFXAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

void MIDIFXAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void MIDIFXAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MIDIFXAudioProcessor();
}
