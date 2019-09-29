#!/bin/bash

set -e

docker_image="permalmberg/smooth:latest"
start_dir="$(pwd)"
source_root="/src"

run_in_docker()
{
	# $1 = script to run
	if [ -z "$1" ]; then
		echo "Missing command for docker"
		exit 1
	else
		docker run -u `id -u` --rm --mount type=bind,source="$start_dir",target=$source_root $@
	fi
}

build_host()
{
	run_in_docker --workdir $source_root $docker_image $source_root/CI/container_scripts/prepare_build_dir.sh
	
	run_in_docker --workdir $source_root $docker_image $source_root/CI/container_scripts/build_smooth_host.sh
}

run_host_tests()
{
	run_in_docker --workdir $source_root/build/host_only_test/unit_tests $docker_image ./unit_test
}

build_esp32()
{
	run_in_docker --workdir $source_root $docker_image $source_root/CI/container_scripts/prepare_build_dir.sh
	run_in_docker --workdir $source_root $docker_image $source_root/CI/container_scripts/build_smooth_esp32.sh
}

build_esp32_test_projects()
{
	for i in $(ls -d test/*/); 
	do 
		current_test="$source_root/${i%%/}"
		echo "######### $current_test #########"
		run_in_docker --workdir $source_root $docker_image $source_root/CI/container_scripts/build_test.sh $current_test
	done
}

if [ ! -d "CI" ]; then
	echo "Please run this script from the source root, e.g. ./CI/main.sh"
	exit 1
else
	build_host
	run_host_tests
	build_esp32
	build_esp32_test_projects
fi
