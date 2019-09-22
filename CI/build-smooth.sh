#!/bin/bash

if [ -z "$1" ]; then
	echo "Please provide a target platform"
	exit 1
else
	if [ -f "CMakeLists.txt" ]; then
		# Run as current user to prevent the build directory being owned by root
	    docker run -u `id -u` --rm --mount type=bind,source="$(pwd)",target=/src permalmberg/smooth:latest $1
	    exit $?
	else
	    echo "Missing CMakeList.txt, please run this script from the root of Smooth."
	    exit 1
	fi
fi
