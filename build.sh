#!/bin/bash

# Exit on error
set -e

echo "🚀 Building Latchy Audio Plugin..."

# Create build directory if it doesn't exist
mkdir -p build

# Add Homebrew's bin to PATH
export PATH="/opt/homebrew/bin:$PATH"

# Configure CMake
echo "📝 Configuring CMake..."
cmake -S . -B build -G "Unix Makefiles"

# Build the project
echo "🔨 Building project..."
cmake --build build --config Release

# Create plugin directories if they don't exist
echo "📂 Creating plugin directories..."
mkdir -p ~/Library/Audio/Plug-Ins/Components
mkdir -p ~/Library/Audio/Plug-Ins/VST3

# Copy the plugins to the plugin directories
echo "📦 Installing plugins..."
if [ -d "build/LittleLatchy_artefacts/AU/LittleLatchy.component" ]; then
    cp -r "build/LittleLatchy_artefacts/AU/LittleLatchy.component" ~/Library/Audio/Plug-Ins/Components/
fi

if [ -d "build/LittleLatchy_artefacts/VST3/LittleLatchy.vst3" ]; then
    cp -r "build/LittleLatchy_artefacts/VST3/LittleLatchy.vst3" ~/Library/Audio/Plug-Ins/VST3/
fi

echo "✨ Build and installation complete!"
echo "🎹 Please restart your DAW to use the plugin."
say "build complete"