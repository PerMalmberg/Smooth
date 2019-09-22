#!/bin/bash

. /src/CI/container_scripts/prepare_idf.sh
. /src/CI/container_scripts/prepare_build_dir.sh

cd /src/build
cmake .. -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=/src/CI/gnu-toolchain.cmake
ninja
