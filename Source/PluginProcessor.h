#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class MIDIFXAudioProcessor : public juce::AudioProcessor
{
public:
    MIDIFXAudioProcessor();
    ~MIDIFXAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    std::atomic<bool> latchEnabled { false };
    int currentLatchedNote { -1 };
    int currentLatchedChannel { 1 };
    bool isNotePlaying { false };
    
    juce::AudioParameterBool* latchParam;
    
    void handleIncomingMidiMessage(const juce::MidiMessage& message, juce::MidiBuffer& processedMidi, int samplePosition);
    void stopCurrentNote(juce::MidiBuffer& processedMidi, int samplePosition);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIFXAudioProcessor)
};
