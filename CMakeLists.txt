cmake_minimum_required(VERSION 3.15)
project(MIDIFX VERSION 1.0.0)

# Add JUCE as a subdirectory
add_subdirectory(JUCE)

# Create the plugin
juce_add_plugin(MIDIFX
    COMPANY_NAME "TerenIO"
    PLUGIN_MANUFACTURER_CODE Tern
    PLUGIN_CODE Mdfx
    FORMATS AU
    PRODUCT_NAME "MIDI FX"
    IS_MIDI_EFFECT TRUE
)

# Add source files
target_sources(MIDIFX
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
)

# Set C++ standard
target_compile_features(MIDIFX PRIVATE cxx_std_17)

# Link with JUCE modules
target_link_libraries(MIDIFX
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
