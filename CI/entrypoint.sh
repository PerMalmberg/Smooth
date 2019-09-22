#!/bin/bash

cd /src

if [ -d build ]; then
	rm -rf build
fi

mkdir build
cd build

if [ "$1" == "esp32" ]; then
	export IDF_TOOLS_PATH=/esp/tools
	. /esp/esp-idf/export.sh
	cmake .. -G "Ninja" -DESP_PLATFORM=1 -DCMAKE_TOOLCHAIN_FILE=/esp/esp-idf/tools/cmake/toolchain-esp32.cmake
	ninja
elif [ "$1" == "host" ]; then
	cmake .. -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=/gnu-toolchain.cmake
	ninja
else
	echo "No valid target system specified"
	exit 1
fi