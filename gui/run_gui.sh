#!/bin/bash
# Launcher script for C++ Complexity Analyzer GUI (Linux/macOS)

echo "Starting C++ Complexity Analyzer GUI..."
echo

# Check if Python 3 is installed
if ! command -v python3 &> /dev/null; then
    echo "Error: Python 3 is not installed"
    echo "Please install Python 3.6 or higher"
    exit 1
fi

# Check if analyzer is built
if [ ! -f "../bin/complexity_analyzer" ]; then
    echo "Warning: Analyzer executable not found!"
    echo "Please build the project first by running: ./build.sh"
    echo
    read -p "Press Enter to continue..."
fi

# Run the GUI
python3 complexity_analyzer_gui.py

if [ $? -ne 0 ]; then
    echo
    echo "Error: Failed to start GUI"
    read -p "Press Enter to exit..."
fi
