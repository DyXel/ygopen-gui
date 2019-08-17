#!/usr/bin/env bash

set -euxo pipefail

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    cd build
    mkdir -p ygopen-gui.app/Contents/MacOS
    cp ygopen-gui ygopen-gui.app/Contents/MacOS
    dylibbundler -x ygopen-gui.app/Contents/MacOS/ygopen-gui -b -d ygopen-gui.app/Contents/MacOS -p @executable_path/
fi
