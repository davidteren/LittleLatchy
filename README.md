# LittleLatchy

A MIDI latch/sustain plugin built with JUCE framework. Version 0.1.0

## Features
- MIDI note latching/sustain functionality
- Multiple plugin formats:
  - Audio Unit (macOS)
  - VST3 (macOS, Windows, Linux)
- Cross-platform compatibility

## Requirements
- JUCE Framework (included as submodule)
- CMake (3.15 or higher)
- Platform-specific requirements:
  - macOS: Xcode
  - Windows: Visual Studio
  - Linux: GCC/Clang and development libraries

## Building
1. Clone this repository
2. Initialize and update git submodules:
   ```bash
   git submodule update --init --recursive
   ```
3. Build using CMake:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```

## Testing
Run the test suite:
```bash
cd build
ctest -C Release --output-on-failure
```

## Installation
After building, the plugin will be available in:
- macOS: 
  - `build/LittleLatchy_artefacts/Release/AU/LittleLatchy.component`
  - `build/LittleLatchy_artefacts/Release/VST3/LittleLatchy.vst3`
- Windows: 
  - `build/LittleLatchy_artefacts/Release/VST3/LittleLatchy.vst3`
- Linux: 
  - `build/LittleLatchy_artefacts/Release/VST3/LittleLatchy.vst3`
