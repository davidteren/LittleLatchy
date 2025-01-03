#include "PluginProcessor.h"
#include "PluginEditor.h"

LatchyAudioProcessorEditor::LatchyAudioProcessorEditor (LatchyAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setSize (400, 300);
}

LatchyAudioProcessorEditor::~LatchyAudioProcessorEditor()
{
}

void LatchyAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Latchy - MIDI Latch Plugin", getLocalBounds(), juce::Justification::centred, 1);
}

void LatchyAudioProcessorEditor::resized()
{
}
