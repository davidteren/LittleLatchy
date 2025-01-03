#!/bin/bash

echo "🧪 Running tests..."

# Navigate to build directory
cd build

# Run tests using ctest
ctest --output-on-failure

echo "✅ Tests complete!"
