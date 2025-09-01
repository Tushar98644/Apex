#!/bin/bash

echo "🔨 Building Trading Engine Python bindings with CMake..."

# Navigate to project root and activate virtual environment
cd "$(dirname "$0")/../.." 
source api_server/.venv/bin/activate

# Verify pybind11 is available
if ! python3 -m pybind11 --includes > /dev/null 2>&1; then
    echo "❌ pybind11 not found! Installing..."
    pip install pybind11
fi

# Navigate to build directory
cd trading_engine
mkdir -p build
cd build

# Clean previous build
rm -f *.so CMakeCache.txt
rm -rf CMakeFiles/

# Run CMake
cmake ..
make -j$(nproc)

if [ $? -eq 0 ]; then
    # Copy to api_server directory
    cp trading_engine*.so ../../api_server/
    echo "✅ Build completed successfully!"
    echo "   🐍 Python module: copied to api_server/"
else
    echo "❌ Build failed!"
    exit 1
fi