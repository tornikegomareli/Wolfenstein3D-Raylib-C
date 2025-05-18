#!/bin/bash

# Exit on error
set -e

# Print commands before executing
set -x

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Build with CMake
cmake ..
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

# Run the game
cd ..
./bin/wolf3d-gpu