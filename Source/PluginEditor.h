#pragma once

#include "PluginProcessor.h"

class LittleLatchyAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit LittleLatchyAudioProcessorEditor (LittleLatchyAudioProcessor&);
    ~LittleLatchyAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    LittleLatchyAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LittleLatchyAudioProcessorEditor)
};
