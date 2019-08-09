# YGOpen GUI [![Build Status](https://travis-ci.org/DyXel/ygopen-gui.svg?branch=master)](https://travis-ci.org/DyXel/ygopen-gui)

The YGOpen project aims to create a free-as-in-freedom, cleanly-engineered, cross-platform Yu-Gi-Oh! Official Card Game simulator, complete with client and server support.

## Prerequisites

- Git
- [CMake](https://cmake.org/download/) 3.12 or higher
- Platform C++ toolchain
  - Windows: [Visual Studio](https://visualstudio.microsoft.com/)
  - macOS and Linux: GCC or Clang

- On macOS, use [Homebrew](https://brew.sh/) to get the prerequisites with `brew install cmake`. If building with GCC, additionally do `brew install gcc`.
- On Windows, set up [`vcpkg`](https://github.com/microsoft/vcpkg). It is recommended to set environment variable `VCPKG_ROOT` to the install location for convenience.

## Dependencies

On Linux, get these dependencies from your package manager or build from source: `glm sdl2 sdl2_image sdl2_ttf protobuf nlohmann-json fmt`.

On Windows, invoke vcpkg to build dependencies from source: `vcpkg install --triplet x64-windows glm sdl2 sdl2-image sdl2-ttf protobuf nlohmann-json fmt`.

On macOS, install dependencies with `brew install glm sdl2 sdl2_image sdl2_ttf protobuf nlohmann-json fmt`.

## Build

Clone the repository if you haven't already, including submodules. Then create the `build` directory for your CMake build tree and switch to it.


```bash
git clone https://github.com/DyXel/ygopen-gui.git
cd ygopen-gui
git submodule update --init --recursive
mkdir -p build
cd build
```

On Linux, do `cmake .. && make`.

On Windows, do `cmake .. -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake` to create the solution files and then build from the generated Visual Studio `.sln` file.

On macOS, do `cmake .. && make` to build with Clang. If building with GCC, do `cmake .. -DCMAKE_CXX_COMPILER=g++-9 -DCMAKE_EXE_LINKER_FLAGS=-lc++ && make` instead.
