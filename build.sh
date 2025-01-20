#!/bin/bash

# Detect OS
OS=$(uname)

if [ "$OS" = "Darwin" ]; then
    # macOS
    if ! command -v brew >/dev/null 2>&1; then
        echo "Homebrew not found. Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    
    if ! brew list sdl2 >/dev/null 2>&1; then
        echo "Installing SDL2..."
        brew install sdl2
    fi
    
    echo "Compiling particle simulator..."
    gcc particle_sim_sdl.c -o particle_sim_sdl -lSDL2 -lm
    
elif [ "$OS" = "Linux" ]; then
    # Linux
    if command -v apt-get >/dev/null 2>&1; then
        # Debian/Ubuntu
        echo "Installing SDL2 development libraries..."
        sudo apt-get update
        sudo apt-get install -y libsdl2-dev
    elif command -v dnf >/dev/null 2>&1; then
        # Fedora
        echo "Installing SDL2 development libraries..."
        sudo dnf install SDL2-devel
    elif command -v pacman >/dev/null 2>&1; then
        # Arch Linux
        echo "Installing SDL2 development libraries..."
        sudo pacman -S sdl2
    else
        echo "Unsupported Linux distribution. Please install SDL2 development libraries manually."
        exit 1
    fi
    
    echo "Compiling particle simulator..."
    gcc particle_sim_sdl.c -o particle_sim_sdl -lSDL2 -lm

else
    echo "Unsupported operating system. Please follow manual compilation instructions:"
    echo "1. Download SDL2 development libraries from https://www.libsdl.org/"
    echo "2. Set up your development environment"
    echo "3. Compile using:"
    echo "   gcc particle_sim_sdl.c -o particle_sim_sdl -I<SDL2_include_path> -L<SDL2_lib_path> -lSDL2 -lm"
fi

if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "Run the simulator with: ./particle_sim_sdl"
else
    echo "Compilation failed. Please check error messages above."
fi
