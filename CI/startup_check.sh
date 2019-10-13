#!/bin/bash

set -e

if [ ! -d "CI" ]; then
  echo "Please run this script from the source root, e.g. ./CI/main.sh"
  exit 1
fi

start_dir="$(pwd)"
source_root="/src"
export start_dir
export source_root