#!/bin/bash

if [ ! -d "CI" ]; then
	echo "Please run this script from the source root, e.g. ./CI/main.sh"
	exit 1
else
#	docker run -u `id -u` --rm --mount type=bind,source="$(pwd)",target=/src --workdir /src permalmberg/smooth:latest /src/CI/container_scripts/build_smooth_host.sh

	# Host unit tests - requires host build
#	docker run -u `id -u` --rm --mount type=bind,source="$(pwd)",target=/src --workdir /src/build/test/linux_unit_tests permalmberg/smooth:latest ./unit_test

	docker run -u `id -u` --rm --mount type=bind,source="$(pwd)",target=/src --workdir /src permalmberg/smooth:latest /src/CI/container_scripts/build_smooth_esp32.sh
fi