#!/usr/bin/env bash

set -euxo pipefail

mkdir -p build
cd build
if [[ "$TRAVIS_OS_NAME" == "windows" ]]; then
	cmake .. -DVCPKG_TARGET_TRIPLET=x64-windows \
		-DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
		-DCMAKE_GENERATOR_PLATFORM=x64 &&
	cmake --build .
fi
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	cmake .. -DYGOPEN_GUI_USE_SYSTEM_FMT=OFF -DYGOPEN_GUI_USE_SYSTEM_JSON=OFF && make;
fi
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
	cmake .. && make;
fi
