#!/bin/bash

echo "Cleaning build artifacts..."

if make clean -C ../build; then
    echo "Build artifacts cleaned successfully."
else
    echo "Failed to clean build artifacts. Please check the Makefile or the environment."
    exit 1
fi

echo "Done."

