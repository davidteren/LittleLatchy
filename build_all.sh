#!/bin/bash
set -e

# Initialize and update git submodules
git submodule update --init --recursive

# Create build directory if it doesn't exist
mkdir -p build

# Build the project
cd build
cmake ..
cmake --build . --config Release

echo "Build process complete. The plugin is now available in the build/LittleLatchy_artefacts/Release directory."