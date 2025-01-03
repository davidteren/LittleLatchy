cmake_minimum_required(VERSION 3.15)
project(LittleLatchy VERSION 0.2.0)

# Add JUCE as a subdirectory
add_subdirectory(JUCE)

# Set JUCE options
set(JUCE_WEB_BROWSER 0)
set(JUCE_USE_CURL 0)
set(JUCE_DISABLE_WEBKIT 1)

# Create the plugin
juce_add_plugin(LittleLatchy
    COMPANY_NAME "TerenIO"
    PLUGIN_MANUFACTURER_CODE Tern
    PLUGIN_CODE Ltch
    FORMATS AU VST3
    PRODUCT_NAME "LittleLatchy"
    IS_MIDI_EFFECT TRUE
    VST3_CATEGORIES "Fx" "MIDI"
)

# Add source files
target_sources(LittleLatchy
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/PluginTests.cpp
)

# Set C++ standard
target_compile_features(LittleLatchy PRIVATE cxx_std_17)

# Add compile definitions
target_compile_definitions(LittleLatchy PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISABLE_WEBKIT=1
    JUCE_USE_WEBKIT=0
)

# Platform-specific settings
if(APPLE)
    set_target_properties(LittleLatchy PROPERTIES
        XCODE_ATTRIBUTE_CLANG_LINK_OBJC_RUNTIME YES
        XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES
    )
elseif(WIN32)
    # Windows-specific settings if needed
endif()

# Link with JUCE modules
target_link_libraries(LittleLatchy
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
juce_add_console_app(LittleLatchyTests PRODUCT_NAME "LittleLatchyTests")

# Add source files
target_sources(LittleLatchyTests PRIVATE
    Source/PluginTests.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

# Configure test executable
target_compile_features(LittleLatchyTests PRIVATE cxx_std_17)

target_compile_definitions(LittleLatchyTests PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JucePlugin_Name="LittleLatchy"
)

# Link JUCE modules
target_link_libraries(LittleLatchyTests PRIVATE
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
add_test(NAME LittleLatchyTests COMMAND LittleLatchyTests)
