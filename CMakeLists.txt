cmake_minimum_required(VERSION 3.15)
project(Latchy VERSION 1.0.0)

# Add JUCE as a subdirectory
add_subdirectory(JUCE)

# Create the plugin
juce_add_plugin(Latchy
    COMPANY_NAME "TerenIO"
    PLUGIN_MANUFACTURER_CODE Tern
    PLUGIN_CODE Ltch
    FORMATS AU
    PRODUCT_NAME "Latchy"
    IS_MIDI_EFFECT TRUE
)

# Add source files
target_sources(Latchy
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/PluginTests.cpp
)

# Set C++ standard
target_compile_features(Latchy PRIVATE cxx_std_17)

# Link with JUCE modules
target_link_libraries(Latchy
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Enable testing
enable_testing()

# Add test executable
juce_add_console_app(LatchyTests PRODUCT_NAME "LatchyTests")

# Add source files
target_sources(LatchyTests PRIVATE
    Source/PluginTests.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

# Configure test executable
target_compile_features(LatchyTests PRIVATE cxx_std_17)

target_compile_definitions(LatchyTests PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JucePlugin_Name="Latchy"
)

# Link JUCE modules
target_link_libraries(LatchyTests PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_audio_plugin_client
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# Add test
add_test(NAME LatchyTests COMMAND LatchyTests)
