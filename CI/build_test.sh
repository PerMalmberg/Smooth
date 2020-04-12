#!/bin/bash

set -e

. ./CI/docker_info.sh
. ./CI/startup_check.sh

run_in_docker() {
  # $1 = script to run
  if [ -z "$1" ]; then
    echo "Missing command for docker"
    exit 1
  else
    docker run --rm --mount type=bind,source="$start_dir",target=$source_root "$@"
  fi
}

build_host() {
  run_in_docker --workdir "$source_root" "$docker_image" "$source_root/CI/container_scripts/prepare_build_dir.sh"
  run_in_docker --workdir "$source_root" "$docker_image" "$source_root/CI/container_scripts/build_smooth_host.sh"
  run_in_docker --workdir "$source_root" "$docker_image" "$source_root/CI/container_scripts/set_build_dir_perms.sh"
}

run_host_tests() {
  run_in_docker --workdir "$source_root/build/test/linux_unit_tests" "$docker_image" "./linux_unit_tests"
}

build_esp32_test_projects() {
  run_in_docker --workdir "$source_root" "$docker_image" "$source_root/CI/container_scripts/prepare_build_dir.sh"
  run_in_docker --workdir "$source_root" "$docker_image" "$source_root/CI/container_scripts/build_smooth_esp32.sh"
  run_in_docker --workdir "$source_root" "$docker_image" "$source_root/CI/container_scripts/set_build_dir_perms.sh"
}

build_host
run_host_tests
build_esp32_test_projects

