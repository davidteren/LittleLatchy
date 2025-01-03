#include "PluginProcessor.h"
#include "PluginEditor.h"

LittleLatchyAudioProcessorEditor::LittleLatchyAudioProcessorEditor (LittleLatchyAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setSize (400, 300);
}

LittleLatchyAudioProcessorEditor::~LittleLatchyAudioProcessorEditor()
{
}

void LittleLatchyAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.drawFittedText ("LittleLatchy - MIDI Latch Plugin", getLocalBounds(), juce::Justification::centred, 1);
}

void LittleLatchyAudioProcessorEditor::resized()
{
}
