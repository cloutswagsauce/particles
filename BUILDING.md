# Building the Particle Physics Simulator

There are two ways to build the simulator:

## 1. Using the Build Script (Linux/macOS)

1. Make the build script executable:
```bash
chmod +x build.sh
```

2. Run the build script:
```bash
./build.sh
```

The script will automatically:
- Detect your operating system
- Install SDL2 if needed (may require sudo/admin password)
- Compile the simulator

## 2. Manual Installation

### Ubuntu/Debian Linux
```bash
# Install SDL2 development libraries
sudo apt-get update
sudo apt-get install libsdl2-dev

# Compile
gcc particle_sim_sdl.c -o particle_sim_sdl -lSDL2 -lm
```

### Fedora Linux
```bash
# Install SDL2 development libraries
sudo dnf install SDL2-devel

# Compile
gcc particle_sim_sdl.c -o particle_sim_sdl -lSDL2 -lm
```

### Arch Linux
```bash
# Install SDL2 development libraries
sudo pacman -S sdl2

# Compile
gcc particle_sim_sdl.c -o particle_sim_sdl -lSDL2 -lm
```

### macOS (using Homebrew)
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install SDL2
brew install sdl2

# Compile
gcc particle_sim_sdl.c -o particle_sim_sdl -lSDL2 -lm
```

### Windows

#### Using MinGW-w64 (recommended)
1. Download and install MinGW-w64 from: https://www.mingw-w64.org/
2. Download SDL2 development libraries for MinGW from: https://github.com/libsdl-org/SDL/releases
   - Look for `SDL2-devel-2.x.x-mingw.zip` (where x.x is the version number)
3. Extract the SDL2 development files:
   - Copy `SDL2-2.x.x/x86_64-w64-mingw32/include/SDL2` to `C:\mingw64\include\`
   - Copy `SDL2-2.x.x/x86_64-w64-mingw32/lib/*` to `C:\mingw64\lib\`
   - Copy `SDL2-2.x.x/x86_64-w64-mingw32/bin/SDL2.dll` to your project directory
4. Open Command Prompt in your project directory and compile:
```bash
gcc particle_sim_sdl.c -o particle_sim_sdl -I"C:\mingw64\include" -L"C:\mingw64\lib" -lmingw32 -lSDL2main -lSDL2 -lm
```

#### Using Visual Studio
1. Download SDL2 development libraries for Visual C++ from: https://github.com/libsdl-org/SDL/releases
   - Look for `SDL2-devel-2.x.x-VC.zip`
2. Create a new Visual Studio project
3. Configure project properties:
   - Add SDL2 include directory
   - Add SDL2 library directory
   - Add SDL2.lib and SDL2main.lib to linker input
4. Copy SDL2.dll to your project's debug/release directory
5. Build the project

## Running the Simulator

After successful compilation, run the simulator:
```bash
./particle_sim_sdl
```

For Windows, make sure SDL2.dll is in the same directory as the executable.

## Troubleshooting

### Common Issues

1. SDL2 not found during compilation:
   - Verify SDL2 is installed correctly
   - Check if pkg-config can find SDL2: `pkg-config --cflags --libs sdl2`
   - Ensure development libraries (headers) are installed

2. Linker errors:
   - Make sure all required libraries are linked (-lSDL2 -lm)
   - For Windows, ensure correct order of libraries (-lmingw32 -lSDL2main -lSDL2)

3. SDL2.dll not found when running:
   - Windows: Copy SDL2.dll to the same directory as the executable
   - Linux: Ensure libsdl2 runtime package is installed
