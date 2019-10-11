#!/bin/bash

. /src/CI/container_scripts/prepare_idf.sh
. /src/CI/container_scripts/prepare_build_dir.sh

set -e

declare -a tests
count=0

# Get list of tests
cd /src/test
for i in $(ls -d */); do
  tests[count]=${i%%/}
  # echo ${test[$count]}
  count=$((count + 1))
done

cd /src/build

cp /src/CMakeLists.txt ./template_CMakeLists.txt

for current in "${tests[@]}"; do

    echo "#######################################"
    echo "Compiling project $current"
    echo "#######################################"
    echo
    cp ./template_CMakeLists.txt /src/CMakeLists.txt
    pattern="s/(selected_test_project)\s+\w+/\1 ${current}/g"

    sed -i -r -e "$pattern" /src/CMakeLists.txt

    cmake .. -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=/src/CI/gnu-toolchain.cmake
    ninja

done

# Clean up
cp ./template_CMakeLists.txt /src/CMakeLists.txt
