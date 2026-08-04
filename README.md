## Hamboy
Hamboy is a GameBoy (DMG) emulator written in C++ and running on SDL2. Hamboy is still under development.

## Current Features
* LR35902 CPU Implementation
* PPU
* Swappable Memory Banks
* SDL2 keyboard input

## Future Updates
* Audio implementation
* Configurable key mapping
* Configurable screen size/resolution
* Loading ROMs via UI
* Save states
* Battery saves

## Download (Windows)
Download the latest release build:
https://github.com/ElricWilder/Hamboy/releases/tag/v0.5
Included in the ZIP:
* Hamboy.exe (Release build)
* SDL2.dll
* README.txt

## Screenshots


## Building From Source
# Requirements:
* C++20 compiler
* CMake 3.20+
* Ninja (recommended)
* SDL2 (via vcpkg)

# Windows (Visual Studio + vcpkg)
Install SDL2: \
```
vcpkg install sdl2:x64-windows
vcpkg install sdl2:x64-windows-release
```

Enable vcpkg integration:
```
vcpkg integrate install
```
Build:
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
Executable will appear in:
```
out/build/x64-Release/Hamboy.exe
```

