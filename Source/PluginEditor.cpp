#include "PluginProcessor.h"
#include "PluginEditor.h"

LittleLatchyAudioProcessorEditor::LittleLatchyAudioProcessorEditor (LittleLatchyAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor(p)
{
    // Custom colors
    backgroundColor = juce::Colour(32, 34, 37);  // Dark grey background
    headerColor = juce::Colour(25, 26, 28);      // Darker header
    buttonColor = juce::Colour(45, 47, 49);      // Button background
    accentColor = juce::Colour(255, 183, 38);    // Gold accent (like ARP)
    textColor = juce::Colours::white;
    
    // Set up latch button
    latchButton.setButtonText("LATCH");
    latchButton.setColour(juce::TextButton::buttonColourId, buttonColor);
    latchButton.setColour(juce::TextButton::buttonOnColourId, accentColor);
    latchButton.setColour(juce::TextButton::textColourOffId, textColor);
    latchButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    latchButton.setClickingTogglesState(true);
    latchButton.setToggleState(p.latchParam->get(), juce::dontSendNotification);
    latchButton.onClick = [this]() {
        if (latchButton.getToggleState()) {
            multiLatchButton.setToggleState(false, juce::sendNotification);
        }
    };
    addAndMakeVisible(latchButton);

    // Set up multi-latch button
    multiLatchButton.setButtonText("MULTI-LATCH");
    multiLatchButton.setColour(juce::TextButton::buttonColourId, buttonColor);
    multiLatchButton.setColour(juce::TextButton::buttonOnColourId, accentColor);
    multiLatchButton.setColour(juce::TextButton::textColourOffId, textColor);
    multiLatchButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    multiLatchButton.setClickingTogglesState(true);
    multiLatchButton.setToggleState(p.multiLatchParam->get(), juce::dontSendNotification);
    multiLatchButton.onClick = [this]() {
        if (multiLatchButton.getToggleState()) {
            latchButton.setToggleState(false, juce::sendNotification);
        }
    };
    addAndMakeVisible(multiLatchButton);

    // Set up panic button
    panicButton.setButtonText("PANIC");
    panicButton.setColour(juce::TextButton::buttonColourId, juce::Colour(40, 40, 40));
    panicButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(255, 69, 58));
    panicButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    panicButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    panicButton.onClick = [this]() { 
        *audioProcessor.panicParam = true;
        startTimer(100);
    };
    addAndMakeVisible(panicButton);

    // Create parameter attachments
    latchAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getState(), "latch", latchButton));
    multiLatchAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.getState(), "multiLatch", multiLatchButton));

    // Set window size
    setSize(300, 200);
}

LittleLatchyAudioProcessorEditor::~LittleLatchyAudioProcessorEditor()
{
    stopTimer();
}

void LittleLatchyAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Draw background
    g.fillAll(backgroundColor);
    
    // Draw header background
    auto headerArea = getLocalBounds().removeFromTop(50);
    g.setColour(headerColor);
    g.fillRect(headerArea);
    
    // Draw plugin title
    g.setColour(textColor);
    auto titleFont = juce::Font(20.0f);
    titleFont.setBold(true);
    g.setFont(titleFont);
    g.drawFittedText("LittleLatchy", headerArea.reduced(15, 0),
                     juce::Justification::centredLeft, 1);
    
    // Draw subtitle
    g.setFont(12.0f);
    g.setColour(textColor.withAlpha(0.7f));
    g.drawFittedText("MIDI LATCH", 
                     headerArea.removeFromRight(80).reduced(5, 0),
                     juce::Justification::centredRight, 1);
}

void LittleLatchyAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    
    // Skip header area
    area.removeFromTop(50);
    
    // Layout buttons
    auto buttonArea = area.reduced(15);
    int buttonHeight = 35;
    int spacing = 10;
    
    // Position mode buttons side by side
    auto modeButtonArea = buttonArea.removeFromTop(buttonHeight);
    int buttonWidth = (modeButtonArea.getWidth() - spacing) / 2;
    
    latchButton.setBounds(modeButtonArea.removeFromLeft(buttonWidth));
    modeButtonArea.removeFromLeft(spacing);
    multiLatchButton.setBounds(modeButtonArea);
    
    // Position panic button at bottom right, smaller size
    buttonArea.removeFromTop(spacing);
    auto panicArea = buttonArea.removeFromTop(buttonHeight).removeFromRight(80);
    panicButton.setBounds(panicArea);
}

void LittleLatchyAudioProcessorEditor::timerCallback()
{
    *audioProcessor.panicParam = false;
    stopTimer();
}
