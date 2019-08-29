#!/usr/bin/env bash

set -euxo pipefail

if [[ "$TRAVIS_OS_NAME" == "windows" ]]; then
	git clone --depth=1 https://github.com/Microsoft/vcpkg.git "$VCPKG_ROOT"
	cd "$VCPKG_ROOT"
	curl --retry 5 --connect-timeout 30 --location --remote-header-name --output installed.zip $VCPKG_CACHE_ZIP_URL
	unzip -uo installed.zip
	powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "& '.\bootstrap-vcpkg.bat'"
	./vcpkg.exe integrate install
	./vcpkg.exe install $VCPKG_LIBS
fi
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	cd lib
	git clone --depth=1 https://github.com/fmtlib/fmt.git &
	git clone --depth=1 https://github.com/nlohmann/json.git
fi
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
	if [[ "$CXX" == "/usr/local/opt/llvm/bin/clang++" ]]; then
		brew install llvm
		./travis/get-osx-sdk.sh $MACOSX_DEPLOYMENT_TARGET
	fi
fi
