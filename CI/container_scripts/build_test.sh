#!/bin/bash

set -e

. /src/CI/container_scripts/prepare_idf.sh

cd $1
mkdir -p build
cd build
cmake .. -G "Ninja" -DESP_PLATFORM=1 -DCMAKE_TOOLCHAIN_FILE=/esp/esp-idf/tools/cmake/toolchain-esp32.cmake
ninja