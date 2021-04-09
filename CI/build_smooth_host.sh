#!/bin/bash

. ./CI/prepare_idf.sh

if [ -d build_host ]; then
	rm -rf build_host
fi
mkdir -p build_host

set -e

declare -a tests
count=0

# Get list of tests
cd test
for i in $(ls -d */); do
  tests[count]=${i%%/}
  # echo ${test[$count]}
  count=$((count + 1))
done

cd ../build_host

cp ../CMakeLists.txt ./original_CMakeLists.txt

for current in "${tests[@]}"; do

    echo "#######################################"
    echo "Compiling project $current"
    echo "#######################################"
    echo
    cp ./original_CMakeLists.txt ../CMakeLists.txt
    pattern="s/(selected_test_project)\s+\w+/\1 ${current}/g"

    sed -i -r -e "$pattern" ../CMakeLists.txt

    cmake .. -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=../CI/gnu-toolchain.cmake
    ninja

done

# Clean up
cp ./original_CMakeLists.txt ../CMakeLists.txt
