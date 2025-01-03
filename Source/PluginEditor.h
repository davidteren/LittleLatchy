#pragma once

#include "PluginProcessor.h"

class LittleLatchyAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    explicit LittleLatchyAudioProcessorEditor (LittleLatchyAudioProcessor&);
    ~LittleLatchyAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // Reference to the processor
    LittleLatchyAudioProcessor& audioProcessor;
    
    juce::ToggleButton latchButton;
    juce::ToggleButton multiLatchButton;
    juce::TextButton panicButton;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> latchAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> multiLatchAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LittleLatchyAudioProcessorEditor)
};
