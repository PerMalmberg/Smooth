#!/bin/bash

. ./CI/prepare_idf.sh

mkdir -p build/esp

set -e

declare -a tests
count=0

# Get list of tests
cd test
for i in $(ls -d */); do
  tests[count]=${i%%/}
  # echo ${test[$count]}
  count=$((count+1))
done

cd ../build/esp

cp ../../CMakeLists.txt ./template_CMakeLists.txt

for current in "${tests[@]}"; do
  if [[ ! "$current" == linux_* ]]; then
    echo "#######################################"
    echo "Compiling project $current"
    echo "#######################################"
    echo
    cp ./template_CMakeLists.txt ../../CMakeLists.txt
    pattern="s/(selected_test_project)\s+\w+/\1 ${current}/g"

    sed -i -r -e "$pattern" ../../CMakeLists.txt

    cmake ../.. -G "Ninja" -DESP_PLATFORM=1 -DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-esp32.cmake
    ninja
  fi
done

# Clean up
cp ./template_CMakeLists.txt ../../CMakeLists.txt