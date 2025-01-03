#!/bin/bash

echo "🧪 Running tests..."

# Navigate to build directory
cd build

# Run tests using ctest with Release configuration
ctest -C Release --output-on-failure

echo "✅ Tests complete!"
