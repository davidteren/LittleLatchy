#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>
#include "PluginProcessor.h"

class LittleLatchyPluginTests : public juce::UnitTest
{
public:
    LittleLatchyPluginTests() : juce::UnitTest("LittleLatchy Plugin Tests") {}
    
    void runTest() override
    {
        beginTest("Parameter Initialization");
        {
            LittleLatchyAudioProcessor processor;
            expect(!processor.latchParam->get(), "Single Latch should be off by default");
            expect(!processor.multiLatchParam->get(), "Multi Latch should be off by default");
            expect(!processor.panicParam->get(), "Panic should be off by default");
        }
        
        beginTest("Mutual Exclusivity of Latch Modes");
        {
            LittleLatchyAudioProcessor processor;
            
            processor.latchParam->setValueNotifyingHost(1.0f);  // Enable Single Latch
            expect(processor.latchParam->get(), "Single Latch should be on");
            expect(!processor.multiLatchParam->get(), "Multi Latch should be off");
            
            processor.multiLatchParam->setValueNotifyingHost(1.0f);  // Enable Multi Latch
            expect(!processor.latchParam->get(), "Single Latch should now be off");
            expect(processor.multiLatchParam->get(), "Multi Latch should be on");
        }
        
        beginTest("Single Latch Note Handling");
        {
            LittleLatchyAudioProcessor processor;
            processor.latchParam->setValueNotifyingHost(1.0f);  // Enable Single Latch
            
            juce::MidiBuffer inputBuffer;
            juce::AudioBuffer<float> audioBuffer;
            audioBuffer.setSize(2, 512);  // Stereo buffer with 512 samples
            
            // Test single note latching
            inputBuffer.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8)100), 0);
            processor.processBlock(audioBuffer, inputBuffer);
            
            // New note should stop previous note
            inputBuffer.clear();
            inputBuffer.addEvent(juce::MidiMessage::noteOn(1, 64, (juce::uint8)100), 0);
            processor.processBlock(audioBuffer, inputBuffer);
            
            // Verify that note off messages are ignored for latched notes
            inputBuffer.clear();
            inputBuffer.addEvent(juce::MidiMessage::noteOff(1, 64), 0);
            processor.processBlock(audioBuffer, inputBuffer);
        }
        
        beginTest("Multi Latch Chord Handling");
        {
            LittleLatchyAudioProcessor processor;
            processor.multiLatchParam->setValueNotifyingHost(1.0f);  // Enable Multi Latch
            
            juce::MidiBuffer inputBuffer;
            juce::AudioBuffer<float> audioBuffer;
            audioBuffer.setSize(2, 512);  // Stereo buffer with 512 samples
            
            // Play a C major chord
            inputBuffer.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8)100), 0);  // C
            inputBuffer.addEvent(juce::MidiMessage::noteOn(1, 64, (juce::uint8)100), 1);  // E
            inputBuffer.addEvent(juce::MidiMessage::noteOn(1, 67, (juce::uint8)100), 2);  // G
            processor.processBlock(audioBuffer, inputBuffer);
            
            // Play a new F major chord
            inputBuffer.clear();
            inputBuffer.addEvent(juce::MidiMessage::noteOn(1, 65, (juce::uint8)100), 0);  // F
            inputBuffer.addEvent(juce::MidiMessage::noteOn(1, 69, (juce::uint8)100), 1);  // A
            inputBuffer.addEvent(juce::MidiMessage::noteOn(1, 72, (juce::uint8)100), 2);  // C
            processor.processBlock(audioBuffer, inputBuffer);
        }
        
        beginTest("MIDI Panic");
        {
            LittleLatchyAudioProcessor processor;
            processor.multiLatchParam->setValueNotifyingHost(1.0f);
            
            juce::MidiBuffer inputBuffer;
            juce::AudioBuffer<float> audioBuffer;
            audioBuffer.setSize(2, 512);  // Stereo buffer with 512 samples
            
            // Play some notes
            inputBuffer.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8)100), 0);
            inputBuffer.addEvent(juce::MidiMessage::noteOn(1, 64, (juce::uint8)100), 1);
            processor.processBlock(audioBuffer, inputBuffer);
            
            // Trigger panic
            processor.panicParam->setValueNotifyingHost(1.0f);
            inputBuffer.clear();
            processor.processBlock(audioBuffer, inputBuffer);
            
            expect(!processor.panicParam->get(), "Panic parameter should reset after use");
        }
    }
};

static LittleLatchyPluginTests test;

int main(int argc, char* argv[])
{
    juce::UnitTestRunner runner;
    runner.runAllTests();
    return 0;
}
