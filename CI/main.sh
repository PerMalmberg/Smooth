#!/bin/bash

set -e

docker_image="permalmberg/smooth:v4.1-dev-281-g96b96ae24"
start_dir="$(pwd)"
source_root="/src"

run_in_docker() {
  # $1 = script to run
  if [ -z "$1" ]; then
    echo "Missing command for docker"
    exit 1
  else
    docker run -u $(id -u) --rm --mount type=bind,source="$start_dir",target=$source_root $@
  fi
}

build_host() {
  run_in_docker --workdir $source_root $docker_image $source_root/CI/container_scripts/prepare_build_dir.sh

  run_in_docker --workdir $source_root $docker_image $source_root/CI/container_scripts/build_smooth_host.sh
}

run_host_tests() {
  run_in_docker --workdir $source_root/build/test/linux_unit_tests $docker_image ./linux_unit_tests
}

build_esp32_test_projects() {
  run_in_docker --workdir $source_root $docker_image $source_root/CI/container_scripts/prepare_build_dir.sh
  run_in_docker --workdir /src permalmberg/smooth:latest /src/CI/container_scripts/build_smooth_esp32.sh
}

if [ ! -d "CI" ]; then
  echo "Please run this script from the source root, e.g. ./CI/main.sh"
  exit 1
else
  build_host
  run_host_tests
  build_esp32_test_projects
fi
