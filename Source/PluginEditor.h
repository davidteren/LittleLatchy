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
    
    // UI Colors
    juce::Colour backgroundColor;
    juce::Colour headerColor;
    juce::Colour buttonColor;
    juce::Colour accentColor;
    juce::Colour textColor;
    
    // UI Components
    juce::TextButton latchButton;
    juce::TextButton multiLatchButton;
    juce::TextButton panicButton;
    
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> latchAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> multiLatchAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LittleLatchyAudioProcessorEditor)
};
