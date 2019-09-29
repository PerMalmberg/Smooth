#!/bin/bash

set -e

. /src/CI/container_scripts/prepare_idf.sh

cd /src/build || exit
cmake .. -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=/src/CI/gnu-toolchain.cmake
ninja
