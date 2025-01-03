#pragma once

#include "PluginProcessor.h"

class MIDIFXAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit MIDIFXAudioProcessorEditor (MIDIFXAudioProcessor&);
    ~MIDIFXAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MIDIFXAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIFXAudioProcessorEditor)
};
