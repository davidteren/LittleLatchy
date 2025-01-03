#include "PluginProcessor.h"
#include "PluginEditor.h"

LittleLatchyAudioProcessorEditor::LittleLatchyAudioProcessorEditor (LittleLatchyAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor(p)
{
    // Set up latch button
    latchButton.setButtonText("Latch");
    latchButton.setToggleState(p.latchParam->get(), juce::dontSendNotification);
    addAndMakeVisible(latchButton);

    // Set up multi-latch button
    multiLatchButton.setButtonText("Multi-Latch");
    multiLatchButton.setToggleState(p.multiLatchParam->get(), juce::dontSendNotification);
    addAndMakeVisible(multiLatchButton);

    // Set up panic button
    panicButton.setButtonText("PANIC!");
    panicButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    panicButton.onClick = [this]() { 
        *audioProcessor.panicParam = true;
        startTimer(100); // Start timer to reset panic button after 100ms
    };
    addAndMakeVisible(panicButton);

    // Create parameter attachments
    latchAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getState(), "latch", latchButton));
    multiLatchAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getState(), "multiLatch", multiLatchButton));

    setSize(400, 200);
}

LittleLatchyAudioProcessorEditor::~LittleLatchyAudioProcessorEditor()
{
    stopTimer();
}

void LittleLatchyAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("LittleLatchy - MIDI Latch Plugin", 
                     getLocalBounds().removeFromTop(30),
                     juce::Justification::centred, 1);
}

void LittleLatchyAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    
    // Header area
    area.removeFromTop(40);
    
    // Layout buttons
    auto buttonArea = area.reduced(20);
    int buttonHeight = 30;
    int spacing = 10;
    
    latchButton.setBounds(buttonArea.removeFromTop(buttonHeight));
    buttonArea.removeFromTop(spacing);
    
    multiLatchButton.setBounds(buttonArea.removeFromTop(buttonHeight));
    buttonArea.removeFromTop(spacing);
    
    panicButton.setBounds(buttonArea.removeFromTop(buttonHeight));
}

void LittleLatchyAudioProcessorEditor::timerCallback()
{
    // Reset panic parameter after timer expires
    *audioProcessor.panicParam = false;
    stopTimer();
}
