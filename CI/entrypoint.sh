#!/bin/bash

. /esp/esp-idf/export.sh

cd /src
mkdir build
cd build
export IDF_PATH=/esp/esp-idf
cmake .. -G "Ninja" -DESP_PLATFORM=1 -DCMAKE_TOOLCHAIN_FILE=/esp/esp-idf/tools/cmake/toolchain-esp32.cmake
ninja

cd /src
rm -rf build
