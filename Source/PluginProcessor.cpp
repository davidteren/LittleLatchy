#include "PluginProcessor.h"
#include "PluginEditor.h"

LatchyAudioProcessor::LatchyAudioProcessor()
    : AudioProcessor (BusesProperties())
{
    addParameter(latchParam = new juce::AudioParameterBool(
        "latch",           // parameterID
        "Single Latch",    // parameter name
        false             // default value
    ));
    
    addParameter(continuousHoldParam = new juce::AudioParameterBool(
        "multiLatch",      // parameterID
        "Multi Latch",     // parameter name
        false             // default value
    ));
    
    addParameter(panicParam = new juce::AudioParameterBool(
        "panic",          // parameterID
        "MIDI Panic",     // parameter name
        false             // default value
    ));
}

LatchyAudioProcessor::~LatchyAudioProcessor()
{
}

const juce::String LatchyAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LatchyAudioProcessor::acceptsMidi() const
{
    return true;
}

bool LatchyAudioProcessor::producesMidi() const
{
    return true;
}

bool LatchyAudioProcessor::isMidiEffect() const
{
    return true;
}

double LatchyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LatchyAudioProcessor::getNumPrograms()
{
    return 1;
}

int LatchyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LatchyAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const juce::String LatchyAudioProcessor::getProgramName (int /*index*/)
{
    return {};
}

void LatchyAudioProcessor::changeProgramName (int /*index*/, const juce::String& /*newName*/)
{
}

void LatchyAudioProcessor::prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/)
{
    // Reset state
    heldNotes.clear();
}

void LatchyAudioProcessor::releaseResources()
{
}

void LatchyAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    
    // Update state from parameters
    latchEnabled.store(latchParam->get());
    continuousHoldEnabled.store(continuousHoldParam->get());
    
    juce::MidiBuffer processedMidi;
    
    // Handle MIDI panic button
    if (panicParam->get())
    {
        sendAllNotesOff(processedMidi, 0);
        *panicParam = false;  // Reset the panic button
        midiMessages.swapWith(processedMidi);
        return;
    }
    
    for (const auto metadata : midiMessages)
    {
        handleIncomingMidiMessage(metadata.getMessage(), processedMidi, metadata.samplePosition);
    }
    
    midiMessages.swapWith(processedMidi);
}

void LatchyAudioProcessor::handleIncomingMidiMessage(const juce::MidiMessage& message, juce::MidiBuffer& processedMidi, int samplePosition)
{
    if (message.isNoteOn())
    {
        HeldNote newNote{message.getNoteNumber(), message.getChannel()};
        bool isMultiLatch = continuousHoldParam->get();
        bool isSingleLatch = latchEnabled.load() && !isMultiLatch;
        
        // Handle Single Latch mode
        if (isSingleLatch)
        {
            // Stop all previous notes in Single Latch mode
            stopAllNotes(processedMidi, samplePosition);
            heldNotes.clear();
            
            // Start the new note
            processedMidi.addEvent(message, samplePosition);
            heldNotes.push_back(newNote);
        }
        // Handle Multi Latch mode
        else if (isMultiLatch)
        {
            static std::vector<HeldNote> currentChordNotes;
            static juce::uint32 lastNoteOnTime = 0;
            const juce::uint32 chordThreshold = 50; // ms threshold for chord detection
            
            juce::uint32 currentTime = juce::Time::getMillisecondCounter();
            
            // If this note is part of a new chord (time gap from last note is significant)
            if (currentTime - lastNoteOnTime > chordThreshold)
            {
                // Stop all previous notes
                stopAllNotes(processedMidi, samplePosition);
                heldNotes.clear();
                currentChordNotes.clear();
            }
            
            // Add the new note to current chord and held notes
            processedMidi.addEvent(message, samplePosition);
            currentChordNotes.push_back(newNote);
            heldNotes.push_back(newNote);
            
            lastNoteOnTime = currentTime;
        }
        else
        {
            // Normal MIDI pass-through when no latch is enabled
            processedMidi.addEvent(message, samplePosition);
        }
    }
    else if (message.isNoteOff())
    {
        HeldNote noteOff{message.getNoteNumber(), message.getChannel()};
        auto it = std::find(heldNotes.begin(), heldNotes.end(), noteOff);
        
        if (it != heldNotes.end())
        {
            // Note is currently held - ignore note-off when in either latch mode
        }
        else
        {
            // Pass through note-off for notes we're not holding
            processedMidi.addEvent(message, samplePosition);
        }
    }
    else if (message.isAllNotesOff() || message.isAllSoundOff())
    {
        sendAllNotesOff(processedMidi, samplePosition);
    }
    else
    {
        // Pass through all other MIDI messages unchanged
        processedMidi.addEvent(message, samplePosition);
    }
}

void LatchyAudioProcessor::stopNote(juce::MidiBuffer& processedMidi, int samplePosition, const HeldNote& note)
{
    processedMidi.addEvent(juce::MidiMessage::noteOff(note.channel, note.noteNumber), samplePosition);
}

void LatchyAudioProcessor::stopAllNotes(juce::MidiBuffer& processedMidi, int samplePosition)
{
    for (const auto& note : heldNotes)
    {
        stopNote(processedMidi, samplePosition, note);
    }
}

void LatchyAudioProcessor::sendAllNotesOff(juce::MidiBuffer& processedMidi, int samplePosition)
{
    // Stop all held notes
    stopAllNotes(processedMidi, samplePosition);
    heldNotes.clear();
    
    // Send all notes off on all MIDI channels
    for (int channel = 1; channel <= 16; ++channel)
    {
        processedMidi.addEvent(juce::MidiMessage::allNotesOff(channel), samplePosition);
        processedMidi.addEvent(juce::MidiMessage::allSoundOff(channel), samplePosition);
    }
}

bool LatchyAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* LatchyAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

void LatchyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = juce::ValueTree("MIDIFXState");
    state.setProperty("latch", latchParam->get(), nullptr);
    state.setProperty("continuousHold", continuousHoldParam->get(), nullptr);
    
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void LatchyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName("MIDIFXState"))
    {
        auto state = juce::ValueTree::fromXml(*xml);
        if (state.hasProperty("latch"))
        {
            *latchParam = state["latch"];
        }
        if (state.hasProperty("continuousHold"))
        {
            *continuousHoldParam = state["continuousHold"];
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LatchyAudioProcessor();
}
