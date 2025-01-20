#!/bin/bash

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    echo "Homebrew not found. Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
else
    echo "Homebrew is already installed."
fi

# Update Homebrew
echo "Updating Homebrew..."
brew update

# Install SDL2
echo "Installing SDL2..."
brew install sdl2

# Compile the program
echo "Compiling particle simulator..."
gcc particle_sim_sdl.c -o particle_sim_sdl -lSDL2 -lm

if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "Run the simulator with: ./particle_sim_sdl"
    chmod +x particle_sim_sdl
else
    echo "Compilation failed. Please check error messages above."
fi
