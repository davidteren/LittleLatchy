#!/bin/bash

# Exit on error
set -e

echo "🚀 Building Latchy Audio Plugin..."

# Create build directory if it doesn't exist
mkdir -p build

# Configure CMake
echo "📝 Configuring CMake..."
cmake -S . -B build

# Build the project
echo "🔨 Building project..."
cmake --build build --config Release

# Create Components directory if it doesn't exist
echo "📂 Creating Components directory..."
mkdir -p ~/Library/Audio/Plug-Ins/Components

# Copy the component to the plugins directory
echo "📦 Installing plugin..."
cp -r build/Latchy_artefacts/Release/AU/Latchy.component ~/Library/Audio/Plug-Ins/Components/

echo "✨ Build and installation complete!"
echo "🎹 Please restart your DAW to use the plugin."
`say build complete`