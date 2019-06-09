#!/bin/bash
pushd `pwd`
mkdir -p build
cd build
cmake .. -G Ninja -DESP_PLATFORM=1 -DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-esp32.cmake -DTARGET=esp32 && \
ninja -j 8 && \
python $IDF_PATH/components/esptool_py/esptool/esptool.py --baud 921600 -p /dev/ttyUSB1 write_flash @flash_project_args && \
popd && \
python $IDF_PATH/tools/idf_monitor.py -p /dev/ttyUSB1 ./build/smooth_test

