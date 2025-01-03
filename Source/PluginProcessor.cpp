#include "PluginProcessor.h"
#include "PluginEditor.h"

MIDIFXAudioProcessor::MIDIFXAudioProcessor()
    : AudioProcessor (BusesProperties())
{
    addParameter(latchParam = new juce::AudioParameterBool(
        "latch",           // parameterID
        "Latch",          // parameter name
        false             // default value
    ));
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

void MIDIFXAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const juce::String MIDIFXAudioProcessor::getProgramName (int /*index*/)
{
    return {};
}

void MIDIFXAudioProcessor::changeProgramName (int /*index*/, const juce::String& /*newName*/)
{
}

void MIDIFXAudioProcessor::prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/)
{
    // Reset latch state
    currentLatchedNote = -1;
    isNotePlaying = false;
}

void MIDIFXAudioProcessor::releaseResources()
{
}

void MIDIFXAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    
    // Update latch state from parameter
    latchEnabled.store(latchParam->get());
    
    juce::MidiBuffer processedMidi;
    
    for (const auto metadata : midiMessages)
    {
        handleIncomingMidiMessage(metadata.getMessage(), processedMidi, metadata.samplePosition);
    }
    
    midiMessages.swapWith(processedMidi);
}

void MIDIFXAudioProcessor::handleIncomingMidiMessage(const juce::MidiMessage& message, juce::MidiBuffer& processedMidi, int samplePosition)
{
    if (message.isNoteOn())
    {
        // If we have a latched note playing and a new note is pressed
        if (latchEnabled.load() && currentLatchedNote != -1)
        {
            // Stop the current note
            stopCurrentNote(processedMidi, samplePosition);
        }
        
        // Start the new note
        processedMidi.addEvent(message, samplePosition);
        
        if (latchEnabled.load())
        {
            currentLatchedNote = message.getNoteNumber();
            currentLatchedChannel = message.getChannel();
            isNotePlaying = true;
        }
    }
    else if (message.isNoteOff() && message.getNoteNumber() == currentLatchedNote)
    {
        if (!latchEnabled.load())
        {
            // If latch is disabled, let the note off through
            processedMidi.addEvent(message, samplePosition);
            currentLatchedNote = -1;
            isNotePlaying = false;
        }
        // If latch is enabled, we ignore the note off message
    }
    else if (message.isAllNotesOff() || message.isAllSoundOff())
    {
        // Always handle all-notes-off messages
        processedMidi.addEvent(message, samplePosition);
        currentLatchedNote = -1;
        isNotePlaying = false;
    }
    else
    {
        // Pass through all other MIDI messages unchanged
        processedMidi.addEvent(message, samplePosition);
    }
}

void MIDIFXAudioProcessor::stopCurrentNote(juce::MidiBuffer& processedMidi, int samplePosition)
{
    if (currentLatchedNote != -1 && isNotePlaying)
    {
        processedMidi.addEvent(juce::MidiMessage::noteOff(currentLatchedChannel, currentLatchedNote), samplePosition);
        isNotePlaying = false;
    }
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
    auto state = juce::ValueTree("MIDIFXState");
    state.setProperty("latch", latchParam->get(), nullptr);
    
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MIDIFXAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName("MIDIFXState"))
    {
        auto state = juce::ValueTree::fromXml(*xml);
        if (state.hasProperty("latch"))
        {
            *latchParam = state["latch"];
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MIDIFXAudioProcessor();
}
