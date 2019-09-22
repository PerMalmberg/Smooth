#!/bin/bash

. /src/CI/container_scripts/prepare_idf.sh
. /src/CI/container_scripts/prepare_build_dir.sh

cd /src/build
cmake .. -G "Ninja" -DESP_PLATFORM=1 -DCMAKE_TOOLCHAIN_FILE=/esp/esp-idf/tools/cmake/toolchain-esp32.cmake
ninja