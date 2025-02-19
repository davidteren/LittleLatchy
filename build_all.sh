#!/bin/bash
set -e

# Initialize and update git submodules
git submodule update --init --recursive

# Create build directory if it doesn't exist
mkdir -p build
# Build the project
cd build || { echo "Failed to enter build directory"; exit 1; }
cmake .. || { echo "CMake configuration failed"; exit 1; }
cmake --build . --config Release || { echo "Build failed"; exit 1; }
cmake --build . --config Release

echo "Build process complete. The plugin is now available in the build/LittleLatchy_artefacts/Release directory."