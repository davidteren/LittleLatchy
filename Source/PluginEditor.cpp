#include "PluginProcessor.h"
#include "PluginEditor.h"

MIDIFXAudioProcessorEditor::MIDIFXAudioProcessorEditor (MIDIFXAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setSize (400, 300);
}

MIDIFXAudioProcessorEditor::~MIDIFXAudioProcessorEditor()
{
}

void MIDIFXAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("MIDI FX Plugin", getLocalBounds(), juce::Justification::centred, 1);
}

void MIDIFXAudioProcessorEditor::resized()
{
}
