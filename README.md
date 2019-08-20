# YGOpen GUI [![Build Status](https://travis-ci.org/DyXel/ygopen-gui.svg?branch=master)](https://travis-ci.org/DyXel/ygopen-gui)

The YGOpen project aims to create a free-as-in-freedom, cleanly-engineered, cross-platform Yu-Gi-Oh! Official Card Game simulator, complete with client and server support.

## Prerequisites

- Git
- [CMake](https://cmake.org/download/) 3.12 or higher
- Platform C++ toolchain
  - Windows: [Visual Studio](https://visualstudio.microsoft.com/)
  - macOS and Linux: GCC or Clang

- On macOS, use [Homebrew](https://brew.sh/) to get the prerequisites with `brew install cmake`. If building on 10.13 or later, but you want compatibility with older versions (back to 10.11 has been tested), you can get `llvm` or `gcc`, which ship with their own C++ standard libraries, from `brew` in the same fashion. Additionally, you might want to install an older SDK to target from [here](https://github.com/phracker/MacOSX-SDKs).
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

On macOS, do `cmake .. && make` to build with Clang. 
If building with LLVM for backward compatibility, do `cmake .. -DCMAKE_CXX_FLAGS="-I/usr/local/opt/llvm/include" -DCMAKE_EXE_LINKER_FLAGS="-L/usr/local/opt/llvm/lib" -DCMAKE_CXX_COMPILER="/usr/local/opt/llvm/bin/clang++" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.11` as your `cmake` command, or set the corresponding environment variables `CXXFLAGS`, `LDFLAGS`, `CXX`, and `MACOSX_DEPLOYMENT_TARGET`. This will also match the SDK version, but you can explicitly set that path with `-DCMAKE_OSX_SYSROOT` and environment variable `SDKROOT`.
If building with GCC, the custom include and library path are not needed. Just set your compiler to `g++-9`. GCC overrides some system headers in the current version, so set `SDKROOT` back to the default at `/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk`. Only the deployment target flag, which controls the `-mmacosx-version-min` compiler flag, will affect the resulting binary compatibility, so using a newer SDK should not be a problem. (Other software is also compiled like this, but we should be careful about compatibility implications or use the LLVM configuration as the standard.)
