#include "PluginProcessor.h"
#include "PluginEditor.h"

LittleLatchyAudioProcessor::LittleLatchyAudioProcessor()
    : AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      latchParam(nullptr),
      multiLatchParam(nullptr),
      panicParam(nullptr),
      lastNoteOnTime(0),
      chordThreshold(50), // ms threshold for chord detection
      state(*this, nullptr, "Parameters", {
          std::make_unique<juce::AudioParameterBool>("latch", "Latch", false),
          std::make_unique<juce::AudioParameterBool>("multiLatch", "Multi-Latch", false),
          std::make_unique<juce::AudioParameterBool>("panic", "Panic", false)
      })
{
    // Get parameters from the state
    latchParam = dynamic_cast<juce::AudioParameterBool*>(state.getParameter("latch"));
    multiLatchParam = dynamic_cast<juce::AudioParameterBool*>(state.getParameter("multiLatch"));
    panicParam = dynamic_cast<juce::AudioParameterBool*>(state.getParameter("panic"));

    // Add parameter listeners
    if (latchParam) latchParam->addListener(this);
    if (multiLatchParam) multiLatchParam->addListener(this);
    if (panicParam) panicParam->addListener(this);
}

LittleLatchyAudioProcessor::~LittleLatchyAudioProcessor()
{
}

const juce::String LittleLatchyAudioProcessor::getName() const
{
    return "LittleLatchy";
}

bool LittleLatchyAudioProcessor::acceptsMidi() const
{
    return true;
}

bool LittleLatchyAudioProcessor::producesMidi() const
{
    return true;
}

bool LittleLatchyAudioProcessor::isMidiEffect() const
{
    return true;
}

double LittleLatchyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LittleLatchyAudioProcessor::getNumPrograms()
{
    return 1;
}

int LittleLatchyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LittleLatchyAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const juce::String LittleLatchyAudioProcessor::getProgramName (int /*index*/)
{
    return {};
}

void LittleLatchyAudioProcessor::changeProgramName (int /*index*/, const juce::String& /*newName*/)
{
}

void LittleLatchyAudioProcessor::prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/)
{
    // Reset state
    heldNotes.clear();
}

void LittleLatchyAudioProcessor::releaseResources()
{
}

void LittleLatchyAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    
    // Update state from parameters
    latchEnabled.store(latchParam->get());
    continuousHoldEnabled.store(multiLatchParam->get());
    
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

void LittleLatchyAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    // Convert double buffer to float for processing
    juce::AudioBuffer<float> floatBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            floatBuffer.setSample(channel, sample, (float)buffer.getSample(channel, sample));
        }
    }

    // Process using the float version
    processBlock(floatBuffer, midiMessages);

    // Copy back to double buffer
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            buffer.setSample(channel, sample, (double)floatBuffer.getSample(channel, sample));
        }
    }
}

void LittleLatchyAudioProcessor::handleIncomingMidiMessage(const juce::MidiMessage& message, juce::MidiBuffer& processedMidi, int samplePosition)
{
    if (message.isNoteOn())
    {
        HeldNote newNote{message.getNoteNumber(), message.getChannel()};
        bool isMultiLatch = continuousHoldEnabled.load();
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

void LittleLatchyAudioProcessor::stopNote(juce::MidiBuffer& processedMidi, int samplePosition, const HeldNote& note)
{
    processedMidi.addEvent(juce::MidiMessage::noteOff(note.channel, note.noteNumber), samplePosition);
}

void LittleLatchyAudioProcessor::stopAllNotes(juce::MidiBuffer& processedMidi, int samplePosition)
{
    for (const auto& note : heldNotes)
    {
        stopNote(processedMidi, samplePosition, note);
    }
}

void LittleLatchyAudioProcessor::sendAllNotesOff(juce::MidiBuffer& processedMidi, int samplePosition)
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

bool LittleLatchyAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* LittleLatchyAudioProcessor::createEditor()
{
    return new LittleLatchyAudioProcessorEditor (*this);
}

void LittleLatchyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto stateTree = state.copyState();
    
    // Add custom properties
    stateTree.setProperty("latchEnabled", latchEnabled.load(), nullptr);
    stateTree.setProperty("continuousHoldEnabled", continuousHoldEnabled.load(), nullptr);
    
    std::unique_ptr<juce::XmlElement> xml(stateTree.createXml());
    copyXmlToBinary(*xml, destData);
}

void LittleLatchyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
    if (xml.get() != nullptr)
    {
        auto stateTree = juce::ValueTree::fromXml(*xml);
        
        // Restore parameter states
        latchEnabled.store(stateTree.getProperty("latchEnabled", false));
        continuousHoldEnabled.store(stateTree.getProperty("continuousHoldEnabled", false));
        
        // Restore plugin state
        state.replaceState(stateTree);
    }
}

void LittleLatchyAudioProcessor::parameterValueChanged(int parameterIndex, float newValue)
{
    // Get parameter that changed
    auto* param = getParameters()[parameterIndex];
    
    if (param == latchParam && newValue > 0.5f)
    {
        // If Single Latch was enabled, disable Multi Latch
        *multiLatchParam = false;
    }
    else if (param == multiLatchParam && newValue > 0.5f)
    {
        // If Multi Latch was enabled, disable Single Latch
        *latchParam = false;
    }
}

void LittleLatchyAudioProcessor::parameterGestureChanged(int, bool) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LittleLatchyAudioProcessor();
}
