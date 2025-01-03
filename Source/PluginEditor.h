#pragma once

#include "PluginProcessor.h"

class LatchyAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit LatchyAudioProcessorEditor (LatchyAudioProcessor&);
    ~LatchyAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    LatchyAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LatchyAudioProcessorEditor)
};
