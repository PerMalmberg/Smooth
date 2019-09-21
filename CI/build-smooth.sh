#!/bin/bash

if [ -f CMakeLists.txt ]; then
    docker run --rm --mount type=bind,source="$(pwd)",target=/src permalmberg/smooth
    exit $?
else
    echo "Missing CMakeList.txt, please run this script from the root of Smooth."
    exit 1
fi
