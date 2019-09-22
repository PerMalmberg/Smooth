#!/bin/bash

export IDF_TOOLS_PATH=/esp/tools

. /esp/esp-idf/export.sh

cd /src
mkdir build
cd build
cmake .. -G "Ninja" -DESP_PLATFORM=1 -DCMAKE_TOOLCHAIN_FILE=/esp/esp-idf/tools/cmake/toolchain-esp32.cmake
ninja
